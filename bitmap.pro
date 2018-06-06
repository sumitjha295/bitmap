TEMPLATE = subdirs
CONFIG += ordered


SUBDIRS = \
        $$PWD/src/bmp \
        $$PWD/src/app \
    tests



app.depends = bmp
