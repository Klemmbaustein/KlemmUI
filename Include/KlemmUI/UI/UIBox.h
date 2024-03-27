#pragma once
#include "../Vector2.h"
#include <set>
#include <vector>

namespace KlemmUI
{
	class ScrollObject;
	class UIButton;
	class UIScrollBox;
	class Window;
	class UIManager;

	/**
	* @brief
	* UIBox class. Parent class for all UI elements.
	*
	* The UIBox itself is not visible.
	*/
	class UIBox
	{
	public:
		bool IsVisible = true;

		/**
		 * @brief
		 * Children align for UIBox.
		 */
		enum class Align
		{
			/// Align from lowest to highest. if Orientation = Horizontal, this means from left to right.
			Default,
			/// Centered align. will act like Align::Default, but children will be put in the center of the box.
			Centered,
			/// Align from highest to lowest. if Orientation = Horizontal, this means from right to left.
			Reverse
		};

		/**
		 * @brief
		 * Describes the border of a UIBox.
		 */
		enum class BorderType
		{
			/// No border.
			None = 0,
			/// Rounded border.
			Rounded = 1,
			/// Darkened edge around the box.
			DarkenedEdge = 2
		};

		enum class SizeMode
		{
			/**
			 * @brief
			 * Default value. Size should be relative to the screen.
			 *
			 * A box with the position x=-1, y=-1 (bottom left corner) and a size of x=1, y=2 will always fill half the screen.
			 *
			 * A box where `size x` = `size y` will only be square if the screen itself is square.
			 */
			ScreenRelative = 0,
			/**
			 * @brief
			 * Size should be relative to the aspect ratio.
			 *
			 * A box where `size x` = `size y` is guaranteed to be square.
			 *
			 * An AspectRelative box with the size x=1, x=1 has the same size as a ScreenRelative box with the size x=1/AspectRatio, y=1.
			 */
			AspectRelative = 1,
			/**
			 * @brief
			 * Size should be relative to the screen resolution.
			 *
			 * A box where `size x` = `size y` is guaranteed to be square.
			 * No matter the screens resolution, the box will always have the same size in pixels.
			 */
			PixelRelative = 2
		};

		UIBox* SetSizeMode(SizeMode NewMode);

		BorderType BoxBorder = BorderType::None;
		float BorderRadius = 0;
		Align HorizontalBoxAlign = Align::Default;
		Align VerticalBoxAlign = Align::Reverse;

		UIBox(bool Horizontal, Vector2f Position);
		virtual ~UIBox();
		virtual void OnAttached();

		/**
		* @brief
		* Invalidates the layout of this UIBox, causing the position and scale of this UIBox and all children to be recalculated on the next UI update.
		* 
		* This function is called by all functions that modify any value that changes the position or scale.
		*/
		void InvalidateLayout();

		/**
		 * @brief
		 * Adds the given UIBox to this UIBox's children.
		 * 
		 * @return
		 * A reference to this UIBox.
		 */
		UIBox* AddChild(UIBox* NewChild);
		UIBox* GetAbsoluteParent();
		void DrawThisAndChildren();
		
		/**
		 * @brief
		 * Deletes all children of this element.
		 */
		void DeleteChildren();

		/**
		 * @brief
		 * Checks if this UIBox is visible in it's hierarchy.
		 * 
		 * ```
		 * UIBox A (visible) -> returns true
		 *   UIBox B (invisible) -> returns false
		 *     UIBox C (visible) -> returns false
		 * ```
		 * 
		 * UIBox C is not visible in the hierarchy since any invisible UIBox will not render it's children.
		 */
		bool IsVisibleInHierarchy();
		bool IsBeingHovered();

		/**
		 * @brief
		 * Sets the maximum size this UIBox can occupy.
		 *
		 * @param NewMaxSize
		 * The new maximum size the UIBox should occupy.
		 *
		 * @return
		 * A reference to this UIBox.
		 */
		UIBox* SetMaxSize(Vector2f NewMaxSize);
		Vector2f GetMaxSize() const;

		/**
		 * @brief
		 * Sets the minimum size this UIBox can occupy.
		 *
		 * @param NewMinSize
		 * The new minimum size the UIBox should occupy.
		 *
		 * @return
		 * A reference to this UIBox.
		 */
		UIBox* SetMinSize(Vector2f NewMinSize);
		Vector2f GetMinSize() const;
		
		/**
		 * @brief
		 * Sets the position of the UIBox.
		 *
		 * @param NewPosition
		 * The new position of the box, where -1, -1 is the bottom left corner of the screen and 1, 1 is the top right corner.
		 * 
		 * @return
		 * A reference to this UIBox.
		 */
		UIBox* SetPosition(Vector2f NewPosition);

		/**
		 * @brief
		 * Gets the position of the UIBox.
		 *
		 * @return
		 * The position of the box, where -1, -1 is the bottom left corner of the screen and 1, 1 is the top right corner.
		 */
		Vector2f GetPosition();
		UIBox* SetPadding(float Up, float Down, float Left, float Right);
		UIBox* SetPadding(float AllDirs);
		UIBox* SetPaddingSizeMode(SizeMode NewSizeMode);
		UIBox* SetTryFill(bool NewTryFill);

		/**
		 * @brief
		 * Sets if the orientation of the children is horizontal.
		 * 
		 * If Orientation is Horizontal, the children of this UIBox will be aligned horizontally.
		 * 
		 * ```
		 * Example:
		 * 
		 *  ____________________________________    ____________________________________
		 * | _______   _______                  |  | _______                            |
		 * ||Child 1| |Child 2|                 |  ||Child 1|                           |
		 * ||_______| |_______|                 |  ||_______|  Parent box               |
		 * |                                    |  | _______   Orientation: Vertical    |
		 * |      Parent box                    |  ||Child 2|                           |
		 * |      Orientation: Horizontal       |  ||_______|                           |
		 * |____________________________________|  |____________________________________|
		 * ```
		 * @return
		 * A reference to this UIBox.
		 */
		UIBox* SetHorizontal(bool IsHorizontal);

		bool GetTryFill() const;
		virtual void OnChildClicked(int Index);
		UIBox* SetBorder(BorderType Type, float Size);

		/**
		 * @brief
		 * Moves this UIBox to the front of the window, on top of all other elements.
		 */
		void MoveToFront();

		/**
		 * @brief
		 * Gets the used size of the UIBox, the size that the UIBox occupies.
		 *
		 * @return
		 * The used size of the box.
		 */
		virtual Vector2f GetUsedSize();
		Vector2f GetScreenPosition() const;
		void SetCurrentScrollObject(UIScrollBox* s);
		void SetCurrentScrollObject(ScrollObject* s);

		/**
		 * @brief
		 * Checks if this UIBox a child of the given parent, or any parent of this UIBox is a child of the parent.
		 * 
		 * @param Parent
		 * The parent which should be checked for.
		 */
		bool IsChildOf(UIBox* Parent);

		/**
		 * @brief
		 * Sets the horizontal align for all children.
		 * 
		 * ```
		 * Example
		 *  ____________________________________    ____________________________________
		 * | _______   _______                  |  |         _______   _______          |
		 * ||Child 1| |Child 2|                 |  |        |Child 1| |Child 2|         |
		 * ||_______| |_______|                 |  |        |_______| |_______|         |
		 * |                                    |  |                                    |
		 * | Parent box                         |  | Parent box                         |
		 * | Horizontal Align: Default          |  | Horizontal Align: Centered         |
		 * |____________________________________|  |____________________________________|
		 * 
		 * ```
		 * 
		 * @return
		 * A reference to this UIBox.
		 */
		UIBox* SetHorizontalAlign(Align NewAlign);

		/**
		 * @brief
		 * Sets the horizontal align for all children.
		 * 
		 * Notes:
		 * - The default vertical align is Align::Reverse, not Align::Default.
		 * - **Align::Reverse aligns boxes from the top down (from 1 to -1)** while Align.Default aligns boxes from the bottom up (from -1 to 1)
		 *   The reason for this is that this corresponds to the way horizontal align works.
		 *   (Default is from -1 to 1 - left to right, Reverse 1 to -1, right to left)
		 * 
		 * ```
		 * Example
		 *  ____________________________________    ____________________________________
		 * | _______   _______                  |  | Vertical Align: Default            |
		 * ||Child 1| |Child 2|                 |  | Parent box                         |
		 * ||_______| |_______|                 |  |                                    |
		 * |                                    |  | _______   _______                  |
		 * | Parent box                         |  ||Child 1| |Child 2|                 |
		 * | Vertical Align: Reverse            |  ||_______| |_______|                 |
		 * |____________________________________|  |____________________________________|
		 * ```
		 * 
		 * @return
		 * A reference to this %UIBox.
		 */
		UIBox* SetVerticalAlign(Align NewAlign);

		/**
		 * @brief
		 * Boolean controlling the collision of the UIBox.
		 * 
		 * If a UIBox has mouse collision, it can be hovered. If it does not have collision, it will be purely visual.
		 * KlemmUI::UIButton has this enabled by default.
		 */
		bool HasMouseCollision = false;
		ScrollObject* CurrentScrollObject = nullptr;

		void GetPadding(Vector2f& UpDown, Vector2f& LeftRight) const;

		/**
		 * @brief
		 * Gets the children of this UIBox.
		 */
		const std::vector<UIBox*>& GetChildren();

		void UpdateElement();
		virtual void UpdateTickState();

		/**
		 * @brief
		 * Gets the parent of this UIBox.
		 */
		UIBox* GetParent();

		/**
		 * @brief
		 * Returns the KlemmUI::Window that this UIBox belongs to.
		 */
		Window* GetParentWindow();
		void GetPaddingScreenSize(Vector2f& UpDown, Vector2f& LeftRight) const;

protected:

		SizeMode BoxSizeMode = SizeMode::ScreenRelative;
		bool ShouldBeTicked = true;
		bool TryFill = false;
		virtual void Update();
		virtual void Draw();
		virtual void Tick();
		void UpdateHoveredState();
		bool IsHovered();
		Vector2f Position;
		Vector2f OffsetPosition;
		Vector2f MaxSize = Vector2(999, 999);
		Vector2f MinSize = Vector2(0, 0);

		float UpPadding = 0.01f;
		float DownPadding = 0.01f;
		float RightPadding = 0.01f;
		float LeftPadding = 0.01f;
		Vector2f Size;
		SizeMode PaddingSizeMode = SizeMode::ScreenRelative;
		Window* ParentWindow = nullptr;

		std::vector<UIBox*> Children;
		UIBox* Parent = nullptr;
		void UpdateSelfAndChildren();
		Vector2f GetLeftRightPadding(const UIBox* Target) const;

		static Vector2f PixelSizeToScreenSize(Vector2f PixelSize, Window* TargetWindow);

	private:
		float GetVerticalOffset();
		float GetHorizontalOffset();
		bool PrevIsVisible = true;
		void UpdateScale();
		void UpdatePosition();
		bool ChildrenHorizontal;


		friend UIManager;
	};
}