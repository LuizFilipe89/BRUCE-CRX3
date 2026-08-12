import datetime
import os


def generate_build_header():
    year = datetime.datetime.now().year
    header_content = f'#pragma once\n#define CURRENT_YEAR {year}\n'

    output_path = os.path.join("include", "current_year.h")
    os.makedirs("include", exist_ok=True)

    # Preserve the timestamp when the year has not changed. Rewriting this
    # widely included header on every invocation invalidates otherwise clean
    # incremental builds and forces PlatformIO to restore/rebuild objects.
    if os.path.exists(output_path):
        with open(output_path, "r", encoding="utf-8") as f:
            if f.read() == header_content:
                return

    with open(output_path, "w", encoding="utf-8", newline="\n") as f:
        f.write(header_content)


def before_build(source, target, env):
    generate_build_header()


# Hook for PlatformIO
def on_pre_build(env):
    before_build(None, None, env)


# Register the pre-build hook
Import("env")
on_pre_build(env)
