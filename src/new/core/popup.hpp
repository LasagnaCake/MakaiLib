#ifndef MAKAI_CORE_POPUP_H
#define MAKAI_CORE_POPUP_H

#include "../compat/ctl.hpp"

namespace Makai::Popup {
	namespace Option {
		const StringList OK				= {"Ok"};
		const StringList YES			= {"Yes"};
		const StringList YES_NO			= {"Yes", "No"};
		const StringList YES_NO_CANCEL	= {"Yes", "No", "Cancel"};
	}

	enum class PopupType {
		PT_INFO,
		PT_WARN,
		PT_ERROR
	};

	/**
	* Invokes a dialog box with a given number of buttons.
	* Returns the selected button (by index).
	* Returns -1 if no button was selected.
	*/
	int showDialog(
		String const& title,
		String const& text,
		StringList const& options = Option::OK,
		PopupType const& type = PopupType::PT_INFO
	);

	void showError(String const& what);
}

#endif // MAKAI_CORE_POPUP_H
