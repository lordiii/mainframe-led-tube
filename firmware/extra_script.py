Import("env")

env.Append(
    LINKFLAGS=[
        "-T\"${PROJECT_DIR}/overlay.ld\"",
    ]
)