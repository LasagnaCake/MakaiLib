#ifndef CTL_OS_FILESYSTEM_H
#define CTL_OS_FILESYSTEM_H

#include <filesystem>
#include "../namespace.hpp"
#include "../container/string.hpp"
#include "../container/error.hpp"
#include "../algorithm/strconv.hpp"

CTL_NAMESPACE_BEGIN

/// @brief Filesystem-related operations.
namespace OS::FS {
	namespace {
		namespace fs = std::filesystem;
	}

	/// @brief Directory path separator.
	enum class PathSeparator: char {
		PS_POSIX	= '/',
		PS_WINDOWS	= '\\'
	};

	#if (_WIN32 || _WIN64 || __WIN32__ || __WIN64__) && !defined(CTL_NO_WINDOWS_PLEASE)
	/// @brief Path separator that the system uses.
	constexpr PathSeparator SEPARATOR = PathSeparator::PS_WINDOWS;
	#else
	/// @brief Path separator that the system uses.
	constexpr PathSeparator SEPARATOR = PathSeparator::PS_POSIX;
	#endif

	/// @brief Checks to see if the specified path exists.
	/// @param path Path to check.
	/// @return Whether the path exists.
	inline bool exists(String const& path) {
		return fs::exists(path.std());
	}

	/// @brief Checks to see if the specified path is a directory.
	/// @param dir Path to check.
	/// @return Whether the path is a directory.
	inline bool isDirectory(String const& dir) {
		return fs::is_directory(dir.std());
	}

	/// @brief Replaces all path separator characters with the specified path separator.
	/// @param path Path to standardize.
	/// @param sep Separator to use.
	/// @return Standardized path.
	constexpr String standardize(String const& path, PathSeparator const& sep) {
		return path.replaced({'\\','/'}, (char)sep);
	}

	/// @brief Replaces all path separator characters with the OS's path separator.
	/// @param path Path to standardize.
	/// @param sep Separator to use.
	/// @return Standardized path.
	constexpr String standardize(String const& path) {
		return standardize(path, SEPARATOR);
	}

	/// @brief Creates a directory, while also creating its parents, if they don't exist.
	/// @param dir Directory to make.
	inline void makeDirectory(String const& dir) {
		if (dir.isNullOrSpaces()) return;
		if (!isDirectory(dir) || !exists(dir)) {
			fs::create_directories(dir.std());
		}
	}

	/// @brief Creates a series of directories, while also creating their parents, if they don't exist.
	/// @param dirs Directories to make.
	inline void makeDirectory(StringList const& dirs) {
		for (auto& d: dirs)
			makeDirectory(d);
	}

	/// @brief Creates a series of directories, while also creating their parents, if they don't exist.
	/// @param dirs Directories to make.
	inline void makeDirectory(StringArguments const& dirs) {
		for (auto& d: dirs)
			makeDirectory(d);
	}

	/// @brief Creates a series of directories, while also creating their parents, if they don't exist.
	/// @tparam ...Args Argument types.
	/// @param ...args Directories to make.
	template <typename... Args>
	inline void makeDirectory(Args const&... args)
	requires (sizeof...(Args) > 1) {
		(..., makeDirectory(toString(args)));
	}

	/// @brief Deletes a file/directory. If it is a directory, it also deletes its contents.
	/// @param path Element to delete.
	inline void remove(String const& path) {
		fs::remove_all(path.std());
	}

	/// @brief Deletes a series of files/directories. If it is a directory, it also deletes its contents.
	/// @param paths Elements to delete.
	inline void remove(StringList const& paths) {
		for (auto& d: paths)
			remove(d);
	}
	
	/// @brief Deletes a series of files/directories. If it is a directory, it also deletes its contents.
	/// @param paths Elements to delete.
	inline void remove(StringArguments const& paths) {
		for (auto& d: paths)
			remove(d);
	}

	/// @brief Deletes a series of files/directories. If it is a directory, it also deletes its contents.
	/// @tparam ...Args Argument types.
	/// @param ...args Elements to delete.
	template <typename... Args>
	inline void remove(Args const&... args) {
		(remove(toString(args)), ...);
	}

	/// @brief Concatenates a two paths together.
	/// @param root Path to concatenate.
	/// @param path Path to concatenate with.
	/// @return Concatenated path.
	constexpr String concatenate(String const& root, String const& path) {
		if (root.empty()) return path;
		String res = root;
		if (!path.empty()) res += "/" + path;
		return res;
	}

	/// @brief Sequentially concatenates a series of paths together.
	/// @param root Path to concatenate.
	/// @param path Paths to concatenate with.
	/// @return Concatenated path.
	constexpr String concatenate(String const& root, StringList const& paths) {
		String res = root;
		for(auto& path: paths) {
			if (!path.empty()) res = concatenate(res, path);
		}
		return res;
	}

	/// @brief Sequentially concatenates a series of paths together.
	/// @param root Path to concatenate.
	/// @param path Paths to concatenate with.
	/// @return Concatenated path.
	constexpr String concatenate(String const& root, StringArguments const& paths) {
		String res = root;
		for(auto& path: paths) {
			if (!path.empty()) res = concatenate(res, path);
		}
		return res;
	}

	/// @brief Sequentially concatenates a series of paths together.
	/// @tparam ...Args Argument types.
	/// @param root Path to concatenate.
	/// @param path First path to concatenate with.
	/// @param ...args Subsequent paths to concatenate with.
	/// @return Concatenated path.
	template<typename... Args>
	constexpr String concatenate(String const& root, String const& path, Args const&... args) {
		String res = concatenate(root, path);
		(..., res.appendBack("/" + toString(args)));
		return res;
	}

	/// @brief Filesystem implementations.
	namespace Impl {
		constexpr String pathDirectory(String const& s) {
			if (s.empty()) return "";
			return (s[0] == '/' ? "" : "/") + s;
		}
	}

	/// @brief Sequentially concatenates a series of paths together.
	/// @tparam ...Args Argument types.
	/// @param root Path to concatenate.
	/// @param ...paths paths to concatenate with.
	/// @return Concatenated path.
	template <typename... Args>
	constexpr String concatenate(String const& root, Args const&... paths) {
		return root + (... + Impl::pathDirectory(toString(paths)));
	}

	/// @brief Returns the file extension.
	/// @param path Path to get from.
	/// @return File extension.
	constexpr String fileExtension(String const& path) {
		auto sp = path.splitAtLast('.');
		return sp.size() > 1 ? sp.back() : "";
	}

	/*constexpr String fileName(String const& path, bool const& removeExtension = false) {
		String result = path.splitAtLast({'\\', '/'}).back();
		return (removeExtension ? result.splitAtLast('.').front() ? result);
	}*/

	/// @brief Returns the directory of the file pointed by the path.
	/// @param path Path to get from.
	/// @param removeExtension Whether to remove the file extension.
	/// @return File name.
	inline String fileName(String const& path, bool removeExtension = false) {
		return String(removeExtension ? fs::path(path.std()).stem().string() : fs::path(path.std()).filename().string());
	}

	/// @brief Returns the parent directory of a given path.
	/// @param path Path to get from.
	/// @return Parent directory.
	constexpr String parentDirectory(String const& path) {
		StringList splitPath = path.splitAtFirst({'\\', '/'});
		if (splitPath.size() > 1)
			return splitPath.front();
		return "";
	}

	/// @brief Returns the directory of the file pointed by the path.
	/// @param path Path to get from.
	/// @return File directory.
	inline String directoryFromPath(String const& path) {
		/*auto const split = path.splitAtLast({'\\', '/'});
		if (split.size() < 2) return "";
		return split.front();*/
		return String(fs::path(path.std()).remove_filename().string());
	}

	/// @brief Returns the path without the parent directory.
	/// @param path Path to get from.
	/// @return Child path.
	constexpr String childPath(String const& path) {
		StringList dirs = path.splitAtFirst({'\\', '/'});
		if (dirs.size() > 1)
			return dirs.back();
		return "";
	}

	/// @brief Directory file tree.
	struct FileTree {
		/// @brief File tree entry.
		struct Entry {
			constexpr Entry() {}

			constexpr Entry(String const& name, String const& path):
				ename(name),
				epath(path) {}

			constexpr Entry(String const& name, String const& path, List<Entry>	children):
				ename(name),
				epath(path),
				children(children),
				folder(true) {}

			constexpr Entry(Entry const& other):
				ename(other.ename),
				epath(other.epath),
				children(other.children),
				folder(other.folder) {}
			
			constexpr Entry(Entry&& other):
				ename(CTL::move(other.ename)),
				epath(CTL::move(other.epath)),
				children(CTL::move(other.children)),
				folder(CTL::move(other.folder)) {}

			constexpr Entry& forEach(Function<void(Entry&)> const& op) {
				for (Entry& e: children)
					op(e);
				return *this;
			}

			constexpr Entry const& forEach(Function<void(Entry const&)> const& op) const {
				for (Entry const& e: children)
					op(e);
				return *this;
			}

			constexpr Entry& operator=(Entry const& other) {
				ename		= other.ename;
				epath		= other.epath;
				folder		= other.folder;
				children	= other.children;
				return *this;
			}

			constexpr Entry& operator=(Entry&& other) {
				ename		= CTL::move(other.ename);
				epath		= CTL::move(other.epath);
				folder		= CTL::move(other.folder);
				children	= CTL::move(other.children);
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
				if (!folder) return StringList{epath};
				StringList files;
				for (Entry const& e: children) {
					if (!e.folder)
						files.pushBack(e.epath);
					else {
						StringList subfiles = e.getAllFiles();
						files.appendBack(subfiles);
					}
				}
				return files;
			}

			constexpr List<Entry> getChildren() const {return children;}

			constexpr operator List<Entry>() const	{return getChildren();	}
			constexpr operator String() const		{return epath;			}

			constexpr bool isFolder() const {return folder;}

			constexpr String name() const {return ename;}
			constexpr String path() const {return epath;}

			constexpr usize empty() const {return children.empty();	}

		private:
			String ename;
			String epath;

			List<Entry>	children	= List<Entry>();
			bool		folder		= false;
		};

		FileTree(String const& path): tree(getStructure(path)) {}

		constexpr FileTree(Entry const& entry): tree(entry) {}

		constexpr FileTree(FileTree const& other): tree(other.tree) {}

		constexpr operator Entry() const {return tree;}

		static inline Entry getStructure(String const& path) {
			if (!exists(path))
				throw Error::InvalidValue("Path does not exist!");
			if (!isDirectory(path)) return Entry(fileName(path), path);
			return Entry(
				String(fs::path(path.std()).stem().string()),
				path,
				getFolderContents(path)
			);
		}

		static inline List<Entry> getFolderContents(String const& folder) {
			if (!isDirectory(folder)) return List<Entry>();
			List<Entry> entries;
			for (auto const& e: fs::directory_iterator(folder.std())) {
				String name = String((e.is_directory()) ? e.path().stem().string() : e.path().filename().string());
				String path = concatenate(folder, name);
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
