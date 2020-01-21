<p align="center"> <h1 align="center">🐉   Lerneaver   🐉</h1> </p>
<p align="center"> <img align="center" src="https://xxxref.com/wp-content/uploads/2020/01/lerneaver_logo.png"> </p>

**Lerneaver** - crossplatform dynamic fuzzing framework for supermans

Current version: 1.0

## Table of contents
- [What is it](#what-is-it)

- [How to build](#how-to-build)

- [How to use](#how-to-use)

## **What is it**
**Lerneaver** is the thing that makes fuzzing process a joy. Let's dont rant about concurrency, platfrom independency, modularity, changing entities state/set on the fly - its obvious that **Lerneaver** has them all and many more, we'll just watch closer on how it works.
There are 3 main types of entities in **Lerneaver** system: 
- Fuzzer 
- Logger
- Outputter

Fuzzer is the thing that generates data, runs in its own thread and is able to init/deinit/stop/resume. Logger somehow processes this data and stores it somewhere (file, stream, mailslot, whatever). And outputter transfers fuzz buffer to target, possibly after some its' postprocessing. It's assumed that each entity object (fuzzer/logger/outputter) is loaded from its' own module (we wont talk about modularity and rebuilding structure during runtime you still remember?).
So all you need to become fuzz superstar is to implement business logic of your fuzzers/loggers/outputters through convinient interfaces and then see your victims constantly crash cos of monstrous fuzzing speed ;3

I like to make shemes, so i visualized basic **Lerneaver** workflow:
<p align="center"> <img align="center" src="https://xxxref.com/wp-content/uploads/2020/01/concept.png"> </p>
Every logger/outputter can be linked with multiple fuzzers at the same time. Here is the example of logger-fuzzer-outputter links configuration:
<p align="center"> <img align="center" src="https://xxxref.com/wp-content/uploads/2020/01/entities_links.png"> </p>

**NOTES:**
- More info about **Lerneaver**: [https://xxxref.com/2020/01/19/meet-lerneaver-superhero-suit-to-perform-heavy-fuzzing-with-ease/](https://xxxref.com/2020/01/19/meet-lerneaver-superhero-suit-to-perform-heavy-fuzzing-with-ease/)

## **How to build**
**Lerneaver** uses CMake as build system.
```
git clone https://github.com/XXXRef/Lerneaver.git
cd Lerneaver
mkdir BUILD
cd BUILD
cmake ..
```

All subprojects will be produced. Ofc you can build **Lerneaver** lib / **Lerneaver** CLI / test modules separately. Or by manipulating *FLAG_ADD_LERNEAVER/FLAG_ADD_CLI_UI/FLAG_ADD_TESTFUZZER/FLAG_ADD_TESTLOGGER/FLAG_ADD_TESTOUTPUTTER* CMake build options.

**NOTES:**
- For now only "*x86-Debug*" build configuration tested

## **How to use**
**Creating entities**
To create your own entity(fuzzer/logger/outputter) copy "inc" folder from appropriate entity subproject(*TestFuzzer/TestLogger/TestOutputter*) and implement entity interface:
- *inc/i_fuzzer.hpp* for fuzzer entity
- *inc/i_logger.hpp* for logger entity
- *inc/i_outputter.hpp* for outputter entity

 *TestFuzzer/TestLogger/TestOutputter* projects also act as example how to implement fuzzing entities.

 **Using Lerneaver as library**

 **Lerneaver** is shipped mainly as static library so you can simply incorporate it into your project.
1. Add/copy "*Lerneaver/src/inc/*" as include folder into your project
2. Include "*Lerneaver/src/inc/fuzzingmanager.hpp*"
3. Create *CFuzzingManager* object
4. Use it's interface in code to perform fuzzing business. Methods names are self-explanatory
5. Link your program with **Lerneaver** library (*Lerneaver.lib/Lerneaver.a/...*)

**Using CLI_UI**

CLI_UI is command-line interface to **Lerneaver**.
- Add fuzzer with fuzzer ID = < fuzzerID > (simply string) and from module with path = < fuzzer_module_path >
```
ADD <fuzzerID> <fuzzer_module_path>
```
- Remove fuzzer with fuzzer ID = < fuzzerID >
```
RM <fuzzerID>
```
- Init fuzzer with fuzzer ID = < fuzzerID > and from config file with path = < fuzzer_cfg_file_path >
```
INIT <fuzzerID> <fuzzer_cfg_file_path>
```
- Deinit fuzzer with fuzzer ID = < fuzzerID >
```
DEINIT <fuzzerID>
```
- Start fuzzer with fuzzer ID = < fuzzerID >
```
START <fuzzerID>
```
- Stop fuzzer with fuzzer ID = < fuzzerID >
```
STOP <fuzzerID>
```
- Add logger with logger ID = < loggerID > (simply string) and from module with path = < logger_module_path >
```
ADD_LOGGER <loggerID> <logger_module_path>
```
- Remove logger with logger ID = < loggerID >
```
RM_LOGGER <loggerID>
```
- Init logger with logger ID = < loggerID > and from config file with path = < logger_cfg_file_path >
```
INIT_LOGGER <loggerID> <logger_cfg_file_path>
```
- Deinit logger with logger ID = < loggerID >
```
DEINIT_LOGGER <loggerID>
```
- Link logger with logger ID = < loggerID > to fuzzer with fuzzer ID = < fuzzerID >
```
LINK_LOGGER <fuzzerID> <loggerID>
```
- Unlink logger with logger ID = < loggerID > from fuzzer with fuzzer ID = < fuzzerID >
```
UNLINK_LOGGER <fuzzerID> <loggerID>
```
- Add outputter with outputter ID = < outputterID > (simply string) and from module with path = < outputter_module_path >
```
ADD_OUTPUTTER <outputterID> <outputter_module_path>
```
- Remove outputter with outputter ID = < outputterID >
```
RM_OUTPUTTER <outputterID>
```
- Init outputter with outputter ID = < outputterID > and from config file with path = < outputter_cfg_file_path >
```
INIT_OUTPUTTER <outputterID> <outputter_cfg_file_path>
```
- Deinit outputter with outputter ID = < outputterID >
```
DEINIT_OUTPUTTER <outputterID>
```
- Link outputter with outputter ID = < outputterID > to fuzzer with fuzzer ID = < fuzzerID >
```
LINK_OUTPUTTER <fuzzerID> <outputterID>
```
- Unlink outputter with outputter ID = < outputterID > from fuzzer with fuzzer ID = < fuzzerID >
```
UNLINK_OUTPUTTER <fuzzerID> <outputterID>
```
- Get fuzzers info (states, linked loggers, linked outputters)
```
LS
```

**NOTES:**
- Config file may contain whatever additional info you want for your entity to initialize itself
