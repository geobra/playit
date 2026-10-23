# playit

Media player for qt based systems mobile devices

# Crosscompile to run within postmarketos

To be documented

# Crosscompile to run within mobian

./create_archive.sh
mv playit-1.0.0.tar.bz2 ../playit_1.0.0.orig.tar.bz2

dpkg-buildpackage -aarm64 -us -uc

