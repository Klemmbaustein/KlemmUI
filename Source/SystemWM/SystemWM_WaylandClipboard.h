#if KLEMMUI_WITH_WAYLAND && !KLEMMUI_CUSTOM_SYSTEMWM
#pragma once
#include <wayland-client-protocol.h>
#include <string>
#include <vector>

namespace kui::systemWM
{
	struct WaylandConnection;

	struct WaylandClipboard
	{
	public:
		struct DataOffer
		{
			wl_data_offer* Offer = nullptr;
			bool Utf8Text = false;
			// TODO: Add drag and drop file support to the library?
			bool UriListText = false;
		};
		std::vector<DataOffer> Offers;

		wl_data_source* SelectionSource = nullptr;
		wl_data_device_manager* DataDeviceManager = nullptr;
		wl_data_device* DataDevice = nullptr;
		wl_data_offer* SelectionOffer = nullptr;
		WaylandConnection* Connection = nullptr;
		std::string ClipboardString;

		void LoadDeviceManager(wl_data_device_manager* Manager);
		void SetSelectionString(std::string NewString);
		std::string GetSelectionString();
	};
}
#endif