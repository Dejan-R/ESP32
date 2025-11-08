
# Registracija main komponente
idf_component_register(
    SRCS "00_ESP32_Project_Structure.c"       # glavna aplikacija
    INCLUDE_DIRS "."    # direktorij sa zaglavljima
    REQUIRES hello_lib)