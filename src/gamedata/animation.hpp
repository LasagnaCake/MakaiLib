namespace Animation {
	template <typename T>
	struct Keyframe {
		T*		value;
		T		to;
		size_t	delay;
		size_t	duration;
		$twn	EaseFunc easing = $twn ease.in.linear;
	};

	template <typename T>
	struct MetaData {
		size_t	index	= 1;
		float	factor	= 0;
		float	step	= 0;
		float	stop	= 0;
		bool	done	= false;
	};

	template <typename T>
	using	Track		= std::vector<Keyframe<T>>;

	template <typename T>
	using	Animation	= std::map<size_t, Track<T>>;

	template <typename T>
	using	TrackData	= std::map<size_t, MetaData<T>>;

	template <typename T>
	struct AnimationPlayer: Entity {
		bool	loop;
		Animation<T> animation;

		void onFrame(float delta) override {
			if (!playing) return;
			for $each(track, animation) {
				processAnimation(track->first, track->second, metadata[track->first], delta);
			}
		}

		void start() {
			if (playing) return;
			playing = true;
			for $each(track, metadata) {
				track->second = {};
			}
		}

		void stop() {
			if (!playing) return;
			playing = false;
		}

	private:
		bool playing = false;

		TrackData<T> metadata;

		void processAnimation(size_t index, Keyframe<T>& anim, MetaData<T>& track, float delta) {
			// If animation is completed, return
			if (!track.done) return;
			// Get start of animation
			T from = animation[index][track.index-1].to;
			// Increment step
			track.step += delta;
			// If begin != end, calculate step
			if (anim.from != anim.to) {
				track.factor = anim.easing(track.step, 0.0f, 1.0f, track.stop);
				*anim.value = Math::lerp(from, anim.to, T(track.factor));
			}
			// Else, set value to end
			else *anim.value = anim.to;
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
