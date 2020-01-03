# Boost.Process

## Idea

* Thin OS wrapper for process "management"
* Supports Windows + POSIX (more?)
* Only supports features that are common to all supported platforms :-(


## Concepts

* Pipes: consist of 2 directed channels (stdin, stdout, stderr)
* Processes: PID, exit code
    * only "hard" termination supported, since this is the only portable way
* Environment: map of variables


## Simple examples

    // "standard" C++
    int result = std::system("g++ main.cpp");

    // Boost.Process
    namespace bp = boost::process;
    int result = bp::system("g++ main.cpp");
    // better:
    int result = bp::system("/usr/bin/g++", "main.cpp");
    // using paths:
    boost::filesystem::path p = "/usr/bin/g++";
    int result = bp::system(p, "main.cpp");
    // alternative: search $PATH / %PATH%
    // (handles .exe/.bat/... properly on Windows)
    boost::filesystem::path p = bp::search_path("g++");
    int result = bp::system(p, "main.cpp");


## More examples

    // spawn & detach
    bp::spawn(bp::search_path("chrome"), "www.boost.org");
    
    // interact with a child process
    bp::child c(bp::search_path("g++"), "main.cpp");
    
    while (c.running())
        do_some_stuff();
    
    c.wait(); // wait for the process to exit
    // alternatives: wait_for(delay), wait_until(time)
    int result = c.exit_code();


## (Synchronous) I/O example

    bp::opstream in;
    bp::ipstream out;
    
    bp::child c("c++filt", bp::std_out > out, bp::std_in < in);
    
    in << "_ZN5boost7process8tutorialE" << endl;
    std::string value;
    out >> value;
    
    c.terminate();


Use pipes to connect 2 processes:

    std::vector<std::string> read_demangled_outline(const std::string& file)
    {
        bp::pipe p;
        bp::ipstream is;
    
        std::vector<std::string> outline;
    
        //we just use the same pipe, so the 
        bp::child nm(bp::search_path("nm"), file,  bp::std_out > p);
        bp::child filt(bp::search_path("c++filt"), bp::std_in < p, bp::std_out > is);
    
        std::string line;
        // when nm finished the pipe closes and c++filt exits
        while (filt.running() && std::getline(is, line))
            outline.push_back(line);
    
        nm.wait();
        filt.wait();
    }


## Good to know

* Error handling: pass an `std::error_code` or catch `std::system_error`
* `bp::child`: kills the process in the destructor if not detached


## Asynchronous I/O

* integrates with Boost.ASIO
* can be used with futures

ASIO example:

    boost::asio::io_service ios;
    std::vector<char> buf(4096);
    
    bp::async_pipe ap(ios);
    
    bp::child c(bp::search_path("g++"), "main.cpp", bp::std_out > ap);
    
    boost::asio::async_read(ap, boost::asio::buffer(buf),
                    [](const boost::system::error_code &ec, std::size_t size){});
    
    ios.run();
    int result = c.exit_code();

Simplified:

    boost::asio::io_service ios;
    std::vector<char> buf(4096);
    
    bp::child c(bp::search_path("g++"), "main.cpp",
                bp::std_out > boost::asio::buffer(buf), ios);
    
    ios.run();
    int result = c.exit_code();

Using futures:

    boost::asio::boost::asio::io_service ios;
    std::future<std::string> data;
    
    bp::child c("g++", "main.cpp",
                bp::std_in.close(),
                bp::std_out > bp::null,
                bp::std_err > data,
                ios);
    
    ios.run(); // this will actually block until the compiler is finished
    
    auto err = data.get();


## Process groups

Children can be grouped for easier management.
Example:

    bp::group g;
    bp::spawn("foo", g);
    bp::spawn("bar", g);

    do_something();

    g.wait();


## Environment

    // get a handle to the current environment
    auto env = boost::this_process::environment();
    // add a variable to the current environment
    env["VALUE_1"] = "foo";
    
    // copy it into an environment separate to the one of this process
    bp::environment env_ = env;
    // append two values to a variable in the new env
    env_["VALUE_2"] += {"bar1", "bar2"};
    
    // launch a process with `env_`
    bp::system("stuff", env_);

More convenient:

    bp::system("stuff", bp::env["VALUE_1"]="foo", bp::env["VALUE_2"]+={"bar1", "bar2"});
