#if KLEMMUI_WITH_WAYLAND && !KLEMMUI_CUSTOM_SYSTEMWM
#include "SystemWM_WaylandClipboard.h"
#include <cstring>
#include <kui/App.h>
#include <unistd.h>
#include "SystemWM_Wayland.h"
#include <fcntl.h>
#include <iostream>

// Based on GLFW's clipboard implementation

using namespace kui;
using namespace kui::systemWM;

static void DataOfferHandleOffer(void* Data, wl_data_offer* offer, const char* MimeType)
{
	WaylandClipboard* c = (WaylandClipboard*)Data;
	for (auto& i : c->Offers)
	{
		if (i.Offer != offer)
		{
			continue;
		}
		std::string MimeTypeStr = MimeType;
		if (MimeTypeStr == "text/plain;charset=utf-8")
			i.Utf8Text = true;
		else if (MimeTypeStr == "text/uri-list")
			i.UriListText = true;

		break;
	}
}

static const wl_data_offer_listener DataOfferListener =
{
	DataOfferHandleOffer
};

static void DataDeviceHandleDataOffer(void* Data, wl_data_device* device, wl_data_offer* offer)
{
	WaylandClipboard* c = (WaylandClipboard*)Data;
	c->Offers.push_back(WaylandClipboard::DataOffer{
		.Offer = offer,
		});
	wl_data_offer_add_listener(offer, &DataOfferListener, c);
}

static void DataDeviceHandleEnter(void* userData, wl_data_device* device, uint32_t serial,
	wl_surface* surface, wl_fixed_t x, wl_fixed_t y, wl_data_offer* offer)
{
}

static void DataDeviceHandleLeave(void* Data, wl_data_device* device)
{
}

static void DataDeviceHandleMotion(void* Data, wl_data_device* device, uint32_t time, wl_fixed_t x, wl_fixed_t y)
{
}

static void DataDeviceHandleDrop(void* Data, wl_data_device* device)
{
}

static void DataDeviceHandleSelection(void* Data, wl_data_device* device, wl_data_offer* offer)
{
	WaylandClipboard* c = (WaylandClipboard*)Data;
	if (c->SelectionOffer)
	{
		wl_data_offer_destroy(c->SelectionOffer);
		c->SelectionOffer = nullptr;
	}

	for (auto i = c->Offers.begin(); i < c->Offers.end(); i++)
	{
		if (i->Offer != offer)
		{
			continue;
		}

		if (i->Utf8Text)
			c->SelectionOffer = offer;
		else
			wl_data_offer_destroy(offer);

		c->Offers.erase(i);
		break;
	}
}

const wl_data_device_listener DataDeviceListener =
{
	DataDeviceHandleDataOffer,
	DataDeviceHandleEnter,
	DataDeviceHandleLeave,
	DataDeviceHandleMotion,
	DataDeviceHandleDrop,
	DataDeviceHandleSelection,
};

static void DataSourceHandleTarget(void* Data, wl_data_source* Source, const char* MimeType)
{
	WaylandClipboard* c = (WaylandClipboard*)Data;

	if (c->SelectionSource != Source)
	{
		app::error::Error("Unknown clipboard Data source");
		return;
	}
}

static void DataSourceHandleSend(void* Data, wl_data_source* Source, const char* MimeType, int fd)
{
	WaylandClipboard* c = (WaylandClipboard*)Data;

	if (c->SelectionSource != Source || std::string("text/plain;charset=utf-8") != MimeType)
	{
		close(fd);
		return;
	}

	ssize_t Position = 0;

	while (Position > 0)
	{
		const ssize_t result = write(fd, c->ClipboardString.c_str() + Position, c->ClipboardString.size() - Position);
		if (result == -1)
		{
			if (errno == EINTR)
				continue;

			app::error::Error(std::string("Writing to the clipboard failed: "), strerror(errno));
			break;
		}

		Position += result;
	}

	close(fd);
}

static void DataSourceHandleCancelled(void* Data, wl_data_source* source)
{
	WaylandClipboard* c = (WaylandClipboard*)Data;
	wl_data_source_destroy(source);

	if (c->SelectionSource != source)
		return;

	c->SelectionSource = nullptr;
}

static const struct wl_data_source_listener DataSourceListener =
{
	DataSourceHandleTarget,
	DataSourceHandleSend,
	DataSourceHandleCancelled,
};

void kui::systemWM::WaylandClipboard::LoadDeviceManager(wl_data_device_manager* Manager)
{
	DataDeviceManager = Manager;
	DataDevice = wl_data_device_manager_get_data_device(DataDeviceManager, Connection->WaylandSeat);
	wl_data_device_add_listener(DataDevice, &DataDeviceListener, this);
}

void kui::systemWM::WaylandClipboard::SetSelectionString(std::string NewString)
{
	if (!Connection)
	{
		return;
	}
	if (SelectionSource)
	{
		wl_data_source_destroy(SelectionSource);
		SelectionSource = nullptr;
	}

	ClipboardString = NewString;

	SelectionSource = wl_data_device_manager_create_data_source(DataDeviceManager);
	if (!SelectionSource)
	{
		app::error::Error("wl_Data_device_manager_create_Data_source returned nullptr");
		return;
	}

	wl_data_source_add_listener(SelectionSource, &DataSourceListener, this);
	wl_data_source_offer(SelectionSource, "text/plain;charset=utf-8");
	wl_data_device_set_selection(DataDevice, SelectionSource, Connection->Serial);
}

static std::string readDataOfferAsString(WaylandConnection* c, wl_data_offer* offer, const char* mimeType)
{
	int fds[2];

	if (pipe2(fds, O_CLOEXEC) == -1)
	{
		app::error::Error("Failed to create pipe for Data offer: " +  std::string(strerror(errno)));
		return "";
	}

	wl_data_offer_receive(offer, mimeType, fds[1]);
	wl_display_flush(c->WaylandDisplay);
	close(fds[1]);

	char* string = NULL;
	size_t size = 0;
	size_t length = 0;

	for (;;)
	{
		const size_t readSize = 4096;
		const size_t requiredSize = length + readSize + 1;
		if (requiredSize > size)
		{
			char* longer = (char*)realloc(string, requiredSize);

			string = longer;
			size = requiredSize;
		}

		const ssize_t result = read(fds[0], string + length, readSize);
		if (result == 0)
			break;
		else if (result == -1)
		{
			if (errno == EINTR)
				continue;

			app::error::Error("Failed to read from Data offer pipe: " + 
				std::string(strerror(errno)));
			close(fds[0]);
			return "";
		}

		length += result;
	}

	close(fds[0]);

	string[length] = '\0';

	std::string OutString = string;
	free(string);
	return OutString;
}
std::string kui::systemWM::WaylandClipboard::GetSelectionString()
{
	if (!SelectionOffer)
	{
		app::error::Error("No clipboard Data available");
		return nullptr;
	}

	if (SelectionSource)
		return ClipboardString;

	ClipboardString = readDataOfferAsString(Connection, SelectionOffer, "text/plain;charset=utf-8");
	return ClipboardString;
}
#endif