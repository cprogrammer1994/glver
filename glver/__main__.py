from . import glver

info = {
	'version' : glver.version,
	'renderer' : glver.renderer,
	'vendor' : glver.vendor,
	'extensions' : glver.extensions,
	'error' : glver.error,
	'info' : glver.info,
}

import sys

executable, *args = sys.argv

if not args:
	args = ['vendor', 'renderer', 'version']

for arg in args:
	if arg not in info:
		raise Exception('%s is not a valid argument.' % arg)

	print(info[arg]())
