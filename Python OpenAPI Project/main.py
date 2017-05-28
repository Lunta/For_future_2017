from search_location import *



def PrintMenu():
    print('')
    print('============Menu============')
    print('s - Search a location info')
    print('q - Quit')
    print('============================')


def main():
    run = True

    while run:
        PrintMenu()

        select = str(input('Select menu: '))

        if select is 's':
            result = SearchLocation()
            if result is not None:
                result.lastest_info()
        elif select is 'q':
            run = False

main()

