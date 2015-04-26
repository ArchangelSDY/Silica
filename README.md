# Silica
An image viewer optimized for Otaku.

# Screenshots

![Main Window](assets/screenshot_main.png)

# Features

* Vim like keybindings.
* Multiple fashion layouts: Compact/Loose/Waterfall.
* Progressive scroll for manga.
* OpenGL accelerated.
* Grouping by similarity.
* Rank system.
* Auto play.
* WebP support with animation.
* Optimized for visiting images on remote device(eg. NAS).
* Open API for plugins.

# Dependencies

* Qt 5.4
* OpenCV

# Build

Silica uses CMake for building.

```shell
$ git clone https://github.com/ArchangelSDY/Silica.git
$ mkdir build
$ cd build
$ cmake ../silica
$ make -j
```

If you want a production build, set `CMAKE_BUILD_TYPE=Release` and `BUILD_PRODUCTION=ON`.
A production build will store profile files in `~/.config/Silica` instead of `~/.config/Silica-Dev` in development build.

```shell
$ cmake ../silica -DCMAKE_BUILD_TYPE=Release -DBUILD_PRODUCTION=ON
```

By default, OpenGL is disabled, you can enable it by setting `ENABLE_OPENGL=ON`.

```shell
$ cmake ../silica -DENABLE_OPENGL=ON
```

Finally, you can make distribution packages after build via:

```shell
$ make package
```

# License
GPL v3.
