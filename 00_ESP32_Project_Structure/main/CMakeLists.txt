# Registracija main komponente
idf_component_register(
    SRCS "main.c"       # glavna aplikacija
    INCLUDE_DIRS "."    # direktorij sa zaglavljima
    REQUIRES hello_lib)
