cmake_minimum_required(VERSION 3.0)
project(Gamma C)

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif ()

# Nie chcemy oglądać komunikatów o procentowym postępie kompilacji.
set_property(GLOBAL PROPERTY RULE_MESSAGES OFF)

# Chcemy zobaczyć polecenia wywołane przez make.
set(CMAKE_VERBOSE_MAKEFILE ON)

# Ustawiamy wspólne opcje kompilowania dla wszystkich wariantów projektu.
set(CMAKE_C_FLAGS "-std=c11 -Wall -Wextra")
# Domyślne opcje dla wariantów Release i Debug są sensowne.
# Jeśli to konieczne, ustawiamy tu inne.
# set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
# set(CMAKE_C_FLAGS_DEBUG "-g")


#set(TEST_FILE
#    src/gamma_test.c
##    ../TESTY/ipp-2020-testy/z2/jm/random/669728508.c
#        )


# Wskazujemy pliki źródłowe.
set(SOURCE_FILES

#    src/gamma_test.c

    ${TEST_FILE}

    src/findUnionLib/findUnion.c src/findUnionLib/findUnion.h
    src/playerLib/player.c src/playerLib/player.h
    src/Avl_Tree_lib/avl_tree.c src/Avl_Tree_lib/avl_tree.h
    src/gammaLib/gammaEngineLib.c src/gammaLib/gammaEngineLib.h
    src/gamma.c src/gamma.h
    )

# Wskazujemy plik wykonywalny.
add_executable(gamma ${SOURCE_FILES})

## --------------------------------------------------
#enable_testing()
## add_subdirectory(testy_do_duzego)
#
#add_executable(gbz1 gbz1.c ${SOURCE_FILES})
#add_test(NAME pierwszy COMMAND gbz1)
## add_test(NAME testy_do_duzego COMMAND *.c)
## --------------------------------------------------


# Dodajemy obsługę Doxygena: sprawdzamy, czy jest zainstalowany i jeśli tak to:
find_package(Doxygen)
if (DOXYGEN_FOUND)
    # Wskazujemy lokalizacją pliku konfiguracyjnego i podajemy jego docelową lokalizację w folderze, gdzie następuje kompilacja.
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    # Dodajemy cel doc: użycie make doc spowoduje wywołanie doxygena z odpowiednim plikiem konfiguracyjnym w folderze kompilacji.
    # Na wyjście zostanie wypisany odpowiedni komentarz.
    add_custom_target(doc
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
    )
endif (DOXYGEN_FOUND)
