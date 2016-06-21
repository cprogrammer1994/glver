from setuptools import setup, Extension

glver = Extension(
	'glver.glver',
	libraries = ['User32'],
	sources = ['source/glver.cpp']
) 

setup(
	name = 'glver',
	version = '1.0.0',
	description = 'Python: Determine OpenGL version.',
	author = 'Szabolcs Dombi',
	author_email = 'cprogrammer1994@gmail.com',
	url = 'https://github.com/cprogrammer1994/glver',
	ext_modules = [glver],
	packages = ['glver'],
)
