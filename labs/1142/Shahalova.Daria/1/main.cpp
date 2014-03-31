int main(int argc, char** argv)
{
    string input_str, command, args;
    
    signal (SIGINT, sigint_handler);
    
    while(1)
    {
        cout << ">";
        getline(cin, input_str);
        if(parse_command(input_str, command, args) == 0)
        {
            if(command.compare("ls") == 0)
            {
                command_ls();
                continue;
            }
            
            if(command.compare("ps") == 0)
            {
                command_ps();
                continue;
            }
            
            if(command.compare("pwd") == 0)
            {
                command_pwd();
                continue;
            }
            
            if(command.compare("kill") == 0)
            {
                command_kill(args);
                continue;
            }
            
            if(command.compare("exit") == 0)
            {
                break;
            }
            
            if(command.compare("help") == 0)
            {
                print_help();
                continue;
            }
            
            run_process(command, args);
        }
    }
    
    return 0;
}