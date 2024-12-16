# ESQLXX

## Guide

Example `main.cpp` file.

```c++
int main()
{
    esqlxx::generator generator("postgresql://user:password@127.0.0.1:5432/database", "table");
    generator.read_inputs();
    generator.generate(true);
    return 0;
}
```

### Generating `code.cpp`

From project root,

```bash
mkdir cmake-build-generate
cd cmake-build-generate
cmake .. .
make
./esqlxx
```

This may take some time for the first time.

### Executing `code.cpp`

From project root,

```bash
mkdir cmake-build-execute
cd cmake-build-execute
cmake -DGENERATE:BOOL=OFF .. .
make
./esqlxx
```

This may take some time for the first time. Note, the generated file needs to be called `code.cpp` to execute.

