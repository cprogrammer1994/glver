rmdir /s /q folder glver.egg-info
rmdir /s /q folder build
rmdir /s /q folder dist
python setup.py install --record files.txt
