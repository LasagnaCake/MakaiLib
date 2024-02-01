namespace Animation {
	namespace {
		using Tweening::Tweenable, Tweening::Tween;
	}

	template <Tweenable T>
	struct Keyframe {
		T				to;
		size_t			delay;
		size_t			duration;
		Ease::EaseMode	easing = Ease::linear;
	};

	template <Tweenable T>
	struct Metadata {
		size_t	index	= 1;
		float	factor	= 0;
		float	step	= 0;
		float	stop	= 0;
		bool	done	= false;
	};

	template <Tweenable T>
	using KeyFrameList	= List<Keyframe<T>>;

	template <Tweenable T>
	struct Track {
		T*				target = nullptr;
		KeyFrameList<T>	keyframes;
		bool			enabled = true;
	};

	template <Tweenable T>
	using	Animation	= HashMap<size_t, Track<T>>;

	template <Tweenable T>
	using	TrackData	= HashMap<size_t, Metadata<T>>;

	template <Tweenable T>
	struct AnimationPlayer: Entity {
		typedef T DataType;

		DERIVED_CLASS(AnimationPlayer, Entity)

		DERIVED_CONSTRUCTOR(AnimationPlayer, Entity, {

		})

		bool	paused = false;
		bool	loop = false;
		float	speed = 1.0;

		Animation<T> animation;

		TypedEvent::Signal<AnimationPlayer<T>*> onAllAnimationsDone = T_SIGNAL(auto& target) {};

		void onFrame(float delta) override {
			if (!playing)	return;
			if (paused)		return;
			bool allDone = true;
			for (auto& [id, track]: animation) {
				// Get track & metadata
				auto& tdata = metadata[id];
				// If no target or disable, skip
				if (!(track.target && track.enabled))
					continue;
				// Check if done processing all tracks
				allDone &= tdata.done;
				// Process animation
				processAnimation(
					id,
					track.keyframes[tdata.index],
					tdata,
					track.target,
					delta
				);
			}
			// If done processing all tracks...
			if (allDone) {
				onAllAnimationsDone(this);
				// Loop if required
				if (loop)	start();
				else		stop();
			}
		}

		void start() {
			if (playing) return;
			paused = playing = true;
			for (auto& [_, md]: metadata) {
				md = Metadata<T>{};
			}
		}

		void stop() {
			if (!playing) return;
			playing = false;
		}

	private:
		bool playing = false;

		TrackData<T> metadata;

		void processAnimation(size_t index, Keyframe<T>& anim, Metadata<T>& track, T* target, float delta) {
			// If animation is completed or no target specified, return
			if (!(track.done && target)) return;
			// Get start of animation
			T from = animation[index].keyframes[track.index-1].to;
			// Increment step
			track.step += speed * delta * 10.0;
			// If begin != end, calculate step
			if (track.step < track.stop) {
				track.factor	= anim.easing(track.step, 0.0f, 1.0f, track.stop);
				*target			= Math::lerp(from, anim.to, T(track.factor));
			}
			// Else, set value to end
			else *target = anim.to;
			// Clamp step to prevent overflow
			track.step = track.step > track.stop ? track.stop : track.step;
			// If reached the end of a keyframe...
			if (track.step >= track.stop) {
				// If track has reached the end, end animation
				if (!(track.index++ < animation[index].keyframes.size())) {
					track.done = true;
					return;
				}
				// Set next track data
				track = {track.index, 0, 0, animation[index].keyframes[track.index].duration};
			}
		}
	};
}
