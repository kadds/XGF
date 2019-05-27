#include "../../Include/InputDefinitions.hpp"
namespace XGF::Input
{
	const KeyBoardKey gDIKToKeyBoardKeyMap[255] =
	{
		KeyBoardKey::None,
		KeyBoardKey::Escape,
		KeyBoardKey::One,
		KeyBoardKey::Two,
		KeyBoardKey::Three,
		KeyBoardKey::Four,
		KeyBoardKey::Five,
		KeyBoardKey::Six,
		KeyBoardKey::Seven,
		KeyBoardKey::Eight,
		KeyBoardKey::Nine,
		KeyBoardKey::Zero,
		KeyBoardKey::Underscore,
		KeyBoardKey::Equals,
		KeyBoardKey::BackSpace,
		KeyBoardKey::Tab,
		KeyBoardKey::Q,
		KeyBoardKey::W,
		KeyBoardKey::E,
		KeyBoardKey::R,
		KeyBoardKey::T,
		KeyBoardKey::Y,
		KeyBoardKey::U,
		KeyBoardKey::I,
		KeyBoardKey::O,
		KeyBoardKey::P,
		KeyBoardKey::LeftBracket,
		KeyBoardKey::RightBracket,
		KeyBoardKey::Enter, // return 
		KeyBoardKey::LeftControl,
		KeyBoardKey::A,
		KeyBoardKey::S,	
		KeyBoardKey::D,
		KeyBoardKey::F,
		KeyBoardKey::G,
		KeyBoardKey::H,
		KeyBoardKey::J,
		KeyBoardKey::K,
		KeyBoardKey::L,
		KeyBoardKey::Semicolon,
		KeyBoardKey::None, // Apostrophe
		KeyBoardKey::None, // grave,
		KeyBoardKey::LeftShift,
		KeyBoardKey::Backslash,
		KeyBoardKey::Z,
		KeyBoardKey::X,
		KeyBoardKey::C,
		KeyBoardKey::V,
		KeyBoardKey::B,
		KeyBoardKey::N,
		KeyBoardKey::M,
		KeyBoardKey::Comma,
		KeyBoardKey::Period,
		KeyBoardKey::Slash,
		KeyBoardKey::RightShift,
		KeyBoardKey::NumPadMultiply,
		KeyBoardKey::LeftAlt,
		KeyBoardKey::SpaceBar,
		KeyBoardKey::CapsLock, 
		KeyBoardKey::F1,
		KeyBoardKey::F2,
		KeyBoardKey::F3,
		KeyBoardKey::F4,
		KeyBoardKey::F5,
		KeyBoardKey::F6,
		KeyBoardKey::F7,
		KeyBoardKey::F8,
		KeyBoardKey::F9,
		KeyBoardKey::F10,
		KeyBoardKey::NumLock,
		KeyBoardKey::ScrollLock,
		KeyBoardKey::NumPadSeven,
		KeyBoardKey::NumPadEight,
		KeyBoardKey::NumPadNine,
		KeyBoardKey::NumPadSubtract,
		KeyBoardKey::NumPadFour,
		KeyBoardKey::NumPadFive,
		KeyBoardKey::NumPadSix,
		KeyBoardKey::NumPadAdd,
		KeyBoardKey::NumPadOne,
		KeyBoardKey::NumPadTwo,
		KeyBoardKey::NumPadThree,
		KeyBoardKey::NumPadZero,
		KeyBoardKey::NumPadDecimal, // .
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None, // oem 102
		KeyBoardKey::F11,
		KeyBoardKey::F12,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None, // F13
		KeyBoardKey::None, // F14
		KeyBoardKey::None, // F15
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None, // KANA
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None, // ABNT_C1
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None, // CONVERT
		KeyBoardKey::None,
		KeyBoardKey::None, // NOCONVERT
		KeyBoardKey::None,
		KeyBoardKey::None, // YEN
		KeyBoardKey::None, // ABNT_C2
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None, // NunberPad Equals
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None, // Prevtrack
		KeyBoardKey::None, // AT
		KeyBoardKey::None, // Colon
		KeyBoardKey::None, // UnderLine
		KeyBoardKey::None, // KanJi
		KeyBoardKey::None, // Stop
		KeyBoardKey::None, // Ax
		KeyBoardKey::None, // Unlabeled
		KeyBoardKey::None,
		KeyBoardKey::None, // NextTrack
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::NumPadEnter, // NumpadEnter
		KeyBoardKey::RightControl, 
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None, // Mute
		KeyBoardKey::None, // Calculator
		KeyBoardKey::None, // Play/ pause
		KeyBoardKey::None,
		KeyBoardKey::None, // Media stop,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None, // VolumeDown
		KeyBoardKey::None,
		KeyBoardKey::None, // VolumeUp
		KeyBoardKey::None,
		KeyBoardKey::None, // WebHome
		KeyBoardKey::None, // NumpadComma
		KeyBoardKey::None,
		KeyBoardKey::NumPadDivide, 
		KeyBoardKey::None,
		KeyBoardKey::None, // SysRq
		KeyBoardKey::RightAlt, 
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::Pause, 
		KeyBoardKey::None,
		KeyBoardKey::Home,
		KeyBoardKey::Up,
		KeyBoardKey::PageUp,
		KeyBoardKey::None,
		KeyBoardKey::Left,
		KeyBoardKey::None,
		KeyBoardKey::Right,
		KeyBoardKey::None,
		KeyBoardKey::End, 
		KeyBoardKey::Down,
		KeyBoardKey::PageDown,
		KeyBoardKey::Insert,
		KeyBoardKey::Delete,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::LeftCommand,
		KeyBoardKey::RightCommand, 
		KeyBoardKey::None, // App Menu,
		KeyBoardKey::None, // System Power
		KeyBoardKey::None, // System Sleep
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None,
		KeyBoardKey::None, // System Wake
		KeyBoardKey::None,
		KeyBoardKey::None, // Web Search
		KeyBoardKey::None, // Web Favorites 
		KeyBoardKey::None, // Web Refresh
		KeyBoardKey::None, // Web Stop
		KeyBoardKey::None, // Web Forward
		KeyBoardKey::None, // Web Back
		KeyBoardKey::None, // My Computer
		KeyBoardKey::None, // Mail
		KeyBoardKey::None, // Media Select

	};

	KeyBoardKey GetKeyFromDIK(unsigned char dikCode)
	{
		return gDIKToKeyBoardKeyMap[dikCode];
	}

	KeyBoardKey GetKeyFromScanCode(unsigned int scanCode)
	{
		if (scanCode == 0xE11D45) {
			KeyBoardKey::Pause;
		}
		if (scanCode & 0xE000)
			return  gDIKToKeyBoardKeyMap[scanCode - 0xDF80];
		else if (scanCode <= 0x7B)
			return  gDIKToKeyBoardKeyMap[scanCode];
		else {
			return KeyBoardKey::None;
		}
	}

}
