#pragma once
#include "../Vec2.h"
#include <set>
#include <vector>
#include <cmath>
#include <kui/UI/UIManager.h>
#include <kui/UISize.h>

namespace kui
{
	class ScrollObject;
	class UIButton;
	class UIScrollBox;
	class Window;

	/**
	* @brief
	* UIBox class. Parent class for all UI elements.
	*
	* The UIBox itself is not visible.
	*/
	class UIBox
	{
	public:
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

		Align HorizontalBoxAlign = Align::Default;
		Align VerticalBoxAlign = Align::Reverse;

		/**
		 * @brief
		 * Constructs a UIBox with either horizontal or vertical child align with the given position.
		 *
		 * @param Horizontal
		 * If true, the UIBox's children will be laid out horizontally. If false, they will be laid out vertically.
		 * See kui::UIBox::SetHorizontal() for more details.
		 *
		 * @param Position
		 * The position of the UIBox.
		 * If the UIBox is a child of another UIBox, this is ignored.
		 */
		UIBox(bool Horizontal, Vec2f Position = 0);
		/**
		 * @brief
		 * UIBox destructor.
		 *
		 * When deleted, a UIBox will also delete all children.
		 */
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
		 * A pointer to this UIBox.
		 */
		UIBox* AddChild(UIBox* NewChild);
		/**
		 * @brief
		 * Gets the root parent of this UIBox.
		 * @return
		 * A pointer to the root parent of this UIBox, or a pointer to this if this UIBox doesn't have a parent.
		 */
		UIBox* GetAbsoluteParent();
		void DrawThisAndChildren(const UIManager::RedrawBox& Box);

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
		 * A pointer to this UIBox.
		 */
		UIBox* SetMaxSize(SizeVec NewMaxSize);
		SizeVec GetMaxSize() const;

		/**
		 * @brief
		 * Sets the minimum size this UIBox can occupy.
		 *
		 * @param NewMinSize
		 * The new minimum size the UIBox can occupy.
		 *
		 * @return
		 * A pointer to this UIBox.
		 */
		UIBox* SetMinSize(SizeVec NewMinSize);
		SizeVec GetMinSize() const;

		/**
		 * @brief
		 * Sets the minimum width this UIBox can occupy.
		 *
		 * @param NewWidth
		 * The new minimum width the UIBox can occupy.
		 *
		 * @return
		 * A pointer to this UIBox.
		 */
		UIBox* SetMinWidth(UISize NewWidth);
		/**
		 * @brief
		 * Sets the minimum height this UIBox can occupy.
		 *
		 * @param NewWidth
		 * The new minimum height the UIBox can occupy.
		 *
		 * @return
		 * A pointer to this UIBox.
		 */
		UIBox* SetMinHeight(UISize NewHeight);

		/**
		 * @brief
		 * Sets the maximum width this UIBox can occupy.
		 *
		 * @param NewWidth
		 * The new maximum width the UIBox can occupy.
		 *
		 * @return
		 * A pointer to this UIBox.
		 */
		UIBox* SetMaxWidth(UISize NewWidth);
		/**
		 * @brief
		 * Sets the maximum height this UIBox can occupy.
		 *
		 * @param NewWidth
		 * The new maximum height the UIBox can occupy.
		 *
		 * @return
		 * A pointer to this UIBox.
		 */
		UIBox* SetMaxHeight(UISize NewHeight);

		/**
		 * @brief
		 * Sets the position of the UIBox.
		 *
		 * @param NewPosition
		 * The new position of the box, where -1, -1 is the bottom left corner
		 * of the screen and 1, 1 is the top right corner.
		 * The position is ignored if this UIBox is a child of another UIBox.
		 *
		 * @return
		 * A pointer to this UIBox.
		 */
		UIBox* SetPosition(Vec2f NewPosition);

		/**
		 * @brief
		 * Gets the position of the UIBox.
		 *
		 * @return
		 * The position of the box, where -1, -1 is the bottom left corner of the screen and 1, 1 is the top right corner.
		 */
		Vec2f GetPosition() const;

		/**
		 * @brief
		 * Sets the padding of a UIBox in each direction.
		 *
		 * Padding works like margin in CSS.
		 *
		 * @return
		 * A pointer to this UIBox.
		 */
		UIBox* SetPadding(UISize Up, UISize Down, UISize Left, UISize Right);

		/**
		 * @brief
		 * Sets the padding of a UIBox, in all directions.
		 *
		 * Padding works like margin in CSS.
		 *
		 * @return
		 * A pointer to this UIBox.
		 */
		UIBox* SetPadding(UISize AllDirs);

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
		 * A pointer to this UIBox.
		 */
		UIBox* SetHorizontal(bool IsHorizontal);

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
		virtual SizeVec GetUsedSize();
		Vec2f GetScreenPosition() const;
		void SetCurrentScrollObject(UIScrollBox* s);
		void SetCurrentScrollObject(ScrollObject* s);

		/**
		 * @brief
		 * Checks if this UIBox a child of the given parent, or any parent of this UIBox is a child of the parent.
		 *
		 * @param Parent
		 * The parent which should be checked for.
		 *
		 * @return
		 * True if this UIBox is a parent to the given UIBox, or any parent of this UIBox is a child of the parent.
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

		ScrollObject* CurrentScrollObject = nullptr;

		/**
		 * @brief
		 * Boolean controlling the collision of the UIBox.
		 *
		 * If a UIBox has mouse collision, it can be hovered. If it does not have collision, it will be purely visual.
		 * kui::UIButton has this enabled by default.
		 */
		bool HasMouseCollision = false;

		/**
		 * @brief
		 * Controls the visibility of the UIBox.
		 */
		bool IsVisible = true;

		/**
		 * @brief
		 * Controls if this element can have keyboard focus.
		 *
		 * An element that can have keyboard focus can be selected by pressing the tab key.
		 */
		bool KeyboardFocusable = false;

		void GetPadding(SizeVec& UpDown, SizeVec& LeftRight) const;

		/**
		 * @brief
		 * Gets the children of this UIBox.
		 */
		const std::vector<UIBox*>& GetChildren();

		/**
		 * @brief
		 * Updates the sizing and position of this element and all it's children.
		 *
		 * Call this if you need the position or size of this element right after it has been created.
		 */
		void UpdateElement();
		virtual void UpdateTickState();

		/**
		 * @brief
		 * Gets the parent of this UIBox.
		 */
		UIBox* GetParent();

		/**
		 * @brief
		 * Returns the kui::Window that this UIBox belongs to.
		 */
		Window* GetParentWindow();
		void GetPaddingScreenSize(Vec2f& UpDown, Vec2f& LeftRight) const;
		static Vec2f PixelSizeToScreenSize(Vec2f PixelSize, Window* TargetWindow);

		/**
		 * @brief
		 * If this is called, the area this element occupies will be redrawn.
		 *
		 * @param Force
		 * If this is true, the element will be redrawn even if it is not visible.
		 */
		void RedrawElement(bool Force = false);

		void SetUpPadding(UISize Value);
		void SetDownPadding(UISize Value);
		void SetLeftPadding(UISize Value);
		void SetRightPadding(UISize Value);

		UIManager::RedrawBox GetRedrawBox() const;

	protected:
		bool ShouldBeTicked = true;
		bool Redrawn = false;
	private:
		bool PrevIsVisible = true;
		bool ChildrenHorizontal = true;
	protected:
		virtual void Update();
		virtual void Draw();
		virtual void Tick();
		void UpdateHoveredState();
		Vec2f Position;
		Vec2f OffsetPosition = NAN;
		SizeVec MaxSize = UISize::Largest();
		SizeVec MinSize = UISize::Smallest();
		size_t LastDrawIndex = 0;
		UISize UpPadding;
		UISize DownPadding;
		UISize RightPadding;
		UISize LeftPadding;
		Vec2f Size;
		Window* ParentWindow = nullptr;

		std::vector<UIBox*> Children;
		UIBox* Parent = nullptr;
		void UpdateSelfAndChildren();

	private:

		void SetOffsetPosition(Vec2f NewPos);

		float GetVerticalOffset();
		float GetHorizontalOffset();
		void UpdateScale();
		void UpdatePosition();


		friend UIManager;
	};
}