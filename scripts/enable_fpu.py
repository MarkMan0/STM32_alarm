Import("env")


env.Append(
  LINKFLAGS=[
    "-mfpu=fpv4-sp-d16",
    "-mfloat-abi=hard",
    "-mthumb"
  ]
)
env.Append(
  CCFLAGS=[
    "-mfpu=fpv4-sp-d16",
    "-mfloat-abi=hard",
    "-mthumb"
  ]
)
