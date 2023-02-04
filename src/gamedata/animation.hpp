namespace Animation {
	namespace {
		using Tween::Tweenable, Tween::EaseFunc, Tween::ease, Tween::Tween;
	}

	template <Tweenable T>
	struct Keyframe {
		T			to;
		size_t		delay;
		size_t		duration;
		EaseFunc	easing = ease.in.linear;
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
	using KeyFrameList	= std::vector<Keyframe<T>>;

	template <Tweenable T>
	struct Track {
		T*				target = nullptr;
		KeyFrameList<T>	keyframes;
		bool			enabled = true;
	};

	template <Tweenable T>
	using	Animation	= std::map<size_t, Track<T>>;

	template <Tweenable T>
	using	TrackData	= std::map<size_t, Metadata<T>>;

	template <Tweenable T>
	struct AnimationPlayer: Entity {
		DERIVED_CLASS(AnimationPlayer, Entity)

		DERIVED_CONSTRUCTOR(AnimationPlayer, Entity, {

		})

		bool	paused = false;
		bool	loop = false;
		float	speed = 1.0;

		Animation<T> animation;

		$tev Signal<AnimationPlayer<T>> onAllAnimationsDone = $tsignal(auto& target) {};

		void onFrame(float delta) override {
			if (!playing)	return;
			if (paused)		return;
			bool allDone = true;
			for $each(anim, animation) {
				// Get track & metadata
				auto& tdata = metadata[anim->first];
				auto& track = anim->second;
				// If no target or disable, skip
				if (!(track.target && track.enabled))
					continue;
				// Check if done processing all tracks
				allDone &= tdata.done;
				// Process animation
				processAnimation(
					anim->first,
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
			for $each(track, metadata) {
				track->second = TrackData<T>{};
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
			T from = animation[index][track.index-1].to;
			// Increment step
			track.step += speed * delta * 10.0;
			// If begin != end, calculate step
			if (anim.from != anim.to) {
				track.factor	= anim.easing(track.step, 0.0f, 1.0f, track.stop);
				*target			= Math::lerp(from, anim.to, T(track.factor));
			}
			// Else, set value to end
			else *target = anim.to;
			// Clamp step to prevent overflow
			track.step = Math::clamp(track.step > track.stop ? track.stop : track.step);
			// If reached the end of a keyframe...
			if (track.step >= track.stop) {
				// If track has reached the end, end animation
				if (!(track.index++ < animation[index].size())) {
					track.done = true;
					return;
				}
				// Set next track data
				track = {track.index, 0, 0, animation[index][track.index].duration};
			}
		}
	};
}
