# UISystem
## About
A simple C++ library that renders UI elements using OpenGL.
Most of the UI code is taken from my [Game Engine](https://github.com/Legofan0807/Klemmgine).

## Installing

GLEW and SDL2 should be installed first.

### With GNU make:

- Download the repository
- `make`
- `make install`

## Examples
### *"Hello, World"*

```cpp
#include <Application.h>

int main()
{
	// Application::Initialize initializes the window. Arguments are: window name, window flags, window resolution.
	// If the window resolution = 0, the window resolution will be 75% of the entire screen size.
	Application::Initialize("Hello, World", Application::NO_RESIZE_BIT, Vector2ui(640, 480));

	// Application::Quit indicates if the window should be closed.
	while (!Application::Quit)
	{
		// Application::UpdateWindow checks for new window events,
		// updates the windows state and renders the window.
		Application::UpdateWindow();
	}
}
```
### *"Hello, World"* with Text
```cpp
#include <Application.h>
#include <UI/UIText.h>

int main()
{
	// Initialize window.
	Application::Initialize("Hello, World. But with text this time!", Application::NO_RESIZE_BIT, Vector2ui(640, 480));

	TextRenderer* Text = new TextRenderer("Path/To/Font.ttf");

	// Creates a new UIText that displays the Text "Hello, World"
	// Arguments are: scale, color, displayed text, text renderer.
	UIText* HelloWorldText = new UIText(1, Vector3f32(1), "Hello, World", Text);

	// Set the texts position.
	// 0,0 is the center, -1,-1, is the bottom left corner, 1,1 is the upper right corner
	//
	//      -1, 1                 1, 1
	//            ---------------
	//            |             |
	//            |     0,0     |
	//      -1, 0 |      +      | 1, 0
	//            |             |
	//            |             |
	//            ---------------
	//      -1,-1                 1,-1
	//
	HelloWorldText->SetPosition(Vector2f(-0.25, 0));


	// Application::Quit indicates if the window should be closed.
	while (!Application::Quit)
	{
		// Update the Window.
		Application::UpdateWindow();
	}
}
```

<br><br><br><br><br><br><br><br><br><br><br><br>
<br><br><br><br><br><br><br><br><br><br><br><br>
<br><br><br><br><br><br><br><br><br><br><br><br>
<br><br><br><br><br><br><br><br><br><br><br><br>

* ⚡ Blazingly fast