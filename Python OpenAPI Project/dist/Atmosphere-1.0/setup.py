
from distutils.core import setup, Extension

files = ["file.png", 'Administrative_divisions_map_of_South_Korea.svg', 'DLSort.pyd']

setup(name='Atmosphere',
version='1.0',
packages=['Atmosphere'],
package_data = {'Atmosphere' : files },
)


#python.exe setup.py sdist