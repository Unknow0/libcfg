libcfg
======

a simple configuration library

it use json file under /etc/cfg and /etc/cfg.d/*.cfg (recurcively) and ~/.config/cfg

/etc/cfg and ~/.config/cfg will populate directly the root.
/etc/cfg.d/[dir/]\*file.cfg will be added under [dir.]\*file key
