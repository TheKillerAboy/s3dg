## Formatting

Requires `astyle` to be installed, `apt-get install astyle`

```
make format
```

## Testing

Requires `Catch2` to be installed, make sure to use a g++ compiled lib
```
make test
```

## Compiling Project
```
make
```

## Running Project
```
make run
```

## TODO
- Add proper Exception handling
- Add way to enable debug logging level from make file
- Make test runner run all tests in one session, and add custom main for testrunner
- Add proper parser exception messages
- Implement `ASTExprBinOps::execute`, to be able to do arthemtics
- `graph3d` method
