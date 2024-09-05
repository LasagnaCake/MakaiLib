struct DanmakuDialog: public Dialog::DialogPlayer {
	DERIVED_CLASS(DanmakuDialog, Dialog::DialogPlayer)

	DERIVED_CONSTRUCTOR(DanmakuDialog, Dialog::DialogPlayer, {})

	TypedSignal<String const&> onBossEnterRequest	= T_SIGNAL(auto) {};
	TypedSignal<String const&> onBossMoveRequest	= T_SIGNAL(auto) {};
	TypedSignal<String const&> onBossExitRequest	= T_SIGNAL(auto) {};

	virtual void onAction(Dialog::Message const& msg) override {
		Dialog::DialogPlayer::onAction(msg);
		if (msg.title == "@:bossenter:")	actionBossEnter(msg);
		if (msg.title == "@:bossexit:")		actionBossExit(msg);
		if (msg.title == "@:bossmove:")		actionBossMove(msg);
	}

private:
	void actionBossEnter(Dialog::Message const& msg) {
		onBossEnterRequest(msg.text);
		nextMessage();
	}


	void actionBossExit(Dialog::Message const& msg) {
		onBossExitRequest(msg.text);
		nextMessage();
	}


	void actionBossMove(Dialog::Message const& msg) {
		onBossMoveRequest(msg.text);
		nextMessage();
	}
};
