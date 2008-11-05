require 'mkmf'
Config::MAKEFILE_CONFIG["CC"] = "g++"
create_makefile('luxio')
