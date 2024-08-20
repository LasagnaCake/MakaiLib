#include "effect.hpp"

using namespace Makai::Graph;

JSON::JSONData Material::saveImageEffect(Material::Effect::Image& effect, String const& folder, String const& path) {
	JSONData def;
	def["enabled"] = effect.enabled;
	if (effect.image && effect.image->exists()) {
		effect.image->saveToFile(FileSystem::concatenatePath(folder, path));
		def["image"] = {
			{"path", path},
			{"minFilter", effect.image->getMinFilter()},
			{"magFilter", effect.image->getMagFilter()}
		};
	} else def["enabled"] = false;
	return def;
}

Material::Effect::Image Material::loadImageEffect(
	JSON::JSONData& effect,
	String const& sourcepath,
	Texture2D& texture
) {
	try {
		Material::Effect::Image fx;
		fx.enabled = effect["enabled"].get<bool>();
		fx.image = texture = Texture2D::fromJSON(effect["image"], sourcepath);
		return fx;
	} catch (JSON::exception const& e) {
		throw Error::FailedAction(
			"Failed at getting image effect!",
			__FILE__,
			toString(__LINE__),
			"loadImageEffect",
			e.what(),
			"Please check to see if values are correct!"
		);
	}
}
