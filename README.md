![volv](https://github.com/TysonKlein/volv/blob/master/Readme/volv_logo.png)

**volv** is a portable and customizable evolution simulator. **volv** uses AI with predisposed behaviours dictated by organism **DNA** to pit organisms against each other. Each organism moves, eats, attacks, flees, mates, and protects offspring to survive, and as unique species develop, they work together to ensure thier survival.

![screenshot](Readme/volv-screenshot.PNG)

**volv** is in the early stages of development. The [current working build](https://github.com/TysonKlein/volv/blob/master/bin/volv.exe) can be found as an executable in the 'bin' folder.

```
Evolutionary algorithm simulator
Usage:
  volv [OPTION...]

  -h, --help  Print help

 Display options:
  -f, --fullscreen      Fullscreen mode
  -d, --developer_mode  Toggle developer mode - shows AI decision making and
                        desired direction of travel

 Engine options:
      --x_buff arg              Horizontal edge buffer (default: 30)
      --y_buff arg              Vertical edge buffer (default: 30)
  -c, --collision_square_size arg
                                Side length of each hash-table collision
                                square (default: 100)
  -u, --unlimited_framerate     Unlock fame rate

 Simulation options:
  -y, --height arg            Simulaton Height (default: 1500)
  -x, --width arg             Simulaton Width (default: 1800)
  -n, --number_organisms arg  Number of unique organisms to start simulation
                              (default: 100)
  -s, --seed arg              Simulation seed, random seed if not specified
                              (default: Random)
      --food_density arg      Food spawn density (default: 15)
```

