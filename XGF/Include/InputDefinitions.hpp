#pragma once
#include <bitset>
namespace XGF::Input
{

	enum class MouseMode {
		Default,
		Exclusive,
	};
	enum class KeyBoardKey
	{
		None = 0,

		// Character Keys
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,

		// Numbers
		Zero,
		One,
		Two,
		Three,
		Four,
		Five,
		Six,
		Seven,
		Eight,
		Nine,

		// Arrow keys
		Left,
		Up,
		Right,
		Down,

		// Num Pad
		NumPadZero,
		NumPadOne,
		NumPadTwo,
		NumPadThree,
		NumPadFour,
		NumPadFive,
		NumPadSix,
		NumPadSeven,
		NumPadEight,
		NumPadNine,
		NumPadEnter,
		NumPadMultiply,
		NumPadAdd,
		NumPadSubtract,
		NumPadDecimal,
		NumPadDivide,
		NumLock,

		ScrollLock,
		CapsLock,
		Escape,
		SpaceBar,
		PageUp,
		PageDown,
		End,
		Home,
		Insert,
		Delete,
		BackSpace,
		Tab,
		Enter,
		Pause,

		// Function Keys
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,

		// Modifier Keys
		LeftShift,
		RightShift,
		LeftControl,
		RightControl,
		LeftAlt,
		RightAlt,
		LeftCommand,
		RightCommand,

		// Misc Keys
		Semicolon,
		Equals,
		Comma,
		Underscore,
		Period,
		Slash,
		Tilde,
		LeftBracket,
		Backslash,
		RightBracket,
		Quote,

		LengthOfKeyBoarderKey,
	};
	enum class MouseKey {
		Left = 0,
		Right,
		Middle,
		Button1,
		Button2,
		Button3,
		Button4,
		Button5,
	};
	KeyBoardKey GetKeyFromDIK(unsigned char dikCode);
	KeyBoardKey GetKeyFromScanCode(unsigned int scanCode);

	class MouseContent
	{
		typedef long pointerType;
	private:
		std::bitset<32> keys;
		pointerType px, py;
		pointerType dx, dy;

		pointerType scrollZ;
	public:

		void Set(MouseKey index, bool value)
		{
			keys.set((int)index, value);
		}

		bool Get(MouseKey index) const
		{
			return keys.test((int)index);
		}
		void SetDeltaX(pointerType x)
		{
			dx = x;
		}
		void SetDeltaY(pointerType y)
		{
			dy = y;
		}
		void SetPosX(pointerType x)
		{
			px = x;
		}
		void SetPosY(pointerType y)
		{
			py = y;
		}
		void SetScroll(int z)
		{
			scrollZ = z;
		}
		pointerType GetPosX() const
		{
			return px;
		}
		pointerType GetPosY() const
		{
			return py;
		}
		pointerType GetDeltaX() const
		{
			return px;
		}
		pointerType GetDeltaY() const
		{
			return py;
		}
		pointerType GetScroll() const
		{
			return scrollZ;
		}
	};
	struct KeyboardContent
	{
		std::bitset<256> keys;
		void Set(KeyBoardKey index, bool value)
		{
			keys.set((int)index, value);
		}
		bool Get(KeyBoardKey index) const
		{
			return keys.test((int)index);
		}
	};

}