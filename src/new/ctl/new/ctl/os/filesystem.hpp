#ifndef CTL_OS_FILESYSTEM_H
#define CTL_OS_FILESYSTEM_H

#include <filesystem>
#include "../namespace.hpp"
#include "../container/string.hpp"
#include "../container/error.hpp"
#include "../algorithm/strconv.hpp"

CTL_NAMESPACE_BEGIN

namespace OS::FS {
	namespace {
		namespace fs = std::filesystem;
		using namespace Helper;
	}

	enum class PathSeparator: char {
		PS_POSIX	= '/',
		PS_WINDOWS	= '\\'
	};

	#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(_NO_WINDOWS_PLEASE_)
	constexpr PathSeparator SEPARATOR = PathSeparator::PS_WINDOWS;
	#else
	constexpr PathSeparator SEPARATOR = PathSeparator::PS_POSIX;
	#endif

	inline bool exists(String const& path) {
		return fs::exists(path);
	}

	inline bool isDirectory(String const& dir) {
		return fs::is_directory(dir);
	}

	constexpr String standardize(String const& path, PathSeparator const& sep) {
		return path.({'\\','/'}, (char)sep);
	}

	constexpr String standardize(String const& path) {
		return standardize(path, SEPARATOR);
	}

	inline void makeDirectory(String const& dir) {
		if (dir.empty()) return;
		if (!isDirectory(dir) || !exists(dir)) {
			fs::create_directories(dir);
		}
	}

	inline void makeDirectory(StringList const& dirs) {
		for (auto& d: dirs)
			makeDirectory(d);
	}

	inline void makeDirectory(StringArguments const& dirs) {
		for (auto& d: dirs)
			makeDirectory(d);
	}

	template <typename... Args>
	inline void makeDirectory(Args const&... args) {
		(makeDirectory(toString(args)), ...);
	}

	constexpr String concatenate(String const& root, String const& path) {
		if (root.empty()) return path;
		String res = root;
		if (!path.empty()) res += "/" + path;
		return res;
	}

	constexpr String concatenate(String const& root, StringList const& paths) {
		String res = root;
		for(auto& path: paths) {
			if (!path.empty()) res = concatenate(res, path);
		}
		return res;
	}

	constexpr String concatenate(String const& root, StringArguments const& paths) {
		String res = root;
		for(auto& path: paths) {
			if (!path.empty()) res = concatenate(res, path);
		}
		return res;
	}

	namespace {
		constexpr String getPathDirectory(String const& s) {
			if (s.empty()) return "";
			return (s[0] == '/' ? "" : "/") + s;
		}
	}

	template <typename... Args>
	constexpr String concatenate(String const& root, Args const&... paths) {
		return root + (... + getPathDirectory(toString(paths)));
	}

	constexpr String fileExtension(String const& path) {
		return path.splitAtLast('.')[1];
	}

	/*constexpr String fileName(String const& path, bool const& removeExtension = false) {
		String result = path.splitAtLast({'\\', '/'})[1];
		return (removeExtension ? result.splitAtLast('.')[0] ? result);
	}*/

	inline String getFileName(String const& path, bool removeExtension = false) {
		return toString(removeExtension ? fs::path(path).stem().string() : fs::path(path).filename().string());
	}

	constexpr String parentDirectory(String const& path) {
		StringList splitPath = path.splitAtFirst({'\\', '/'});
		if (splitPath.size() != 1)
			return splitPath[0];
		return "";
	}

	inline String directoryFromPath(String const& path) {
		return fs::path(path).remove_filename().string();
	}

	constexpr String childPath(String const& path) {
		StringList dirs = path.splitAtFirst({'\\', '/'});
		if (dirs.size() == 1)
			return path;
		return dirs[1];
	}

	struct FileTree {
		struct Entry {
			constexpr Entry(String const& name, String const& path):
				name(name),
				path(path) {}

			constexpr Entry(String const& name, String const& path, List<Entry>	children):
				name(name),
				path(path),
				children(children),
				folder(true) {}

			constexpr Entry(Entry const& other):
				name(other.name),
				path(other.path),
				children(other.children),
				folder(other.folder) {}

			constexpr Entry& forEach(Operation<Entry const&> const& op) {
				for (Entry& e: children)
					op(e);
				return *this;
			}

			constexpr Entry const& forEach(Operation<Entry const&> const& op) const {
				for (Entry const& e: children)
					op(e);
				return *this;
			}

			constexpr auto begin()			{return children.begin();	}
			constexpr auto end()			{return children.end();		}
			constexpr auto cbegin()			{return children.cbegin();	}
			constexpr auto cend()			{return children.cend();	}

			constexpr auto begin() const	{return children.begin();	}
			constexpr auto end() const		{return children.end();		}
			constexpr auto cbegin() const	{return children.cbegin();	}
			constexpr auto cend() const		{return children.cend();	}

			constexpr StringList getAllFiles() const {
				if (!folder) return StringList{path};
				StringList files;
				for (Entry const& e: children) {
					if (!e.folder)
						files.push_back(e.path);
					else {
						StringList subfiles = e.getAllFiles();
						files.insert(files.end(), subfiles.begin(), subfiles.end());
					}
				}
				return files;
			}

			constexpr List<Entry> getChildren() const {return children;}

			constexpr operator List<Entry>() const	{return getChildren();	}
			constexpr operator String() const		{return path;			}

			constexpr bool isFolder() const {return folder;}

			String const name;
			String const path;

		private:
			List<Entry>	children	= List<Entry>();
			bool const	folder		= false;
		};

		FileTree(String const& path): tree(getStructure(path)) {}

		constexpr FileTree(Entry const& entry): tree(entry) {}

		constexpr FileTree(FileTree const& other): tree(other.tree) {}

		constexpr operator Entry() const {return tree;}

		static inline Entry getStructure(String const& path) {
			if (!exists(path))
				throw Error::InvalidValue("Path does not exist!");
			if (!isDirectory(path)) return Entry(getFileName(path), path);
			return Entry(
				fs::path(path).stem().string(),
				path,
				getFolderContents(path)
			);
		}

		static inline List<Entry> getFolderContents(String const& folder) {
			if (!isDirectory(folder)) return List<Entry>();
			List<Entry> entries;
			for (auto const& e: fs::directory_iterator(folder)) {
				String name = (e.is_directory()) ? e.path().stem().string() : e.path().filename().string();
				String path = concatenatePath(folder, name);
				if (e.is_directory())
					entries.pushBack(Entry(name, path, getFolderContents(path)));
				else
					entries.pushBack(Entry(name, path));
			}
			return entries;
		}

		Entry tree;
	};
}

CTL_NAMESPACE_END

#endif // CTL_OS_FILESYSTEM_H
