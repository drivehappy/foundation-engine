import sys
import yaml

# Profiling Modules
import cProfile
import pstats

if __name__ == '__main__':
    # Load up our foundationParams.yaml file
    stream = file('../../data/foundationParams.yaml', 'r')
    programs = yaml.load(stream)['program']
    
    if (len(sys.argv) != 2):
        print "    [Python] Error: Format (python main.py [program]}"
        exit(1)

    if (programs.has_key(sys.argv[1])):
        parameters = programs[sys.argv[1]]
        FOUNDATION_MODULE_DIR  = parameters['folder']
        FOUNDATION_MODULE_NAME = parameters['script']
        PROFILE = parameters['profiling']

        # Load the module
        sys.path.append('../../code/' + FOUNDATION_MODULE_DIR)
        print "Running Module: " + FOUNDATION_MODULE_NAME
        try:
            exec("import " + FOUNDATION_MODULE_NAME)
        except:
            print "Unexpected error:", sys.exc_info()[0]
            raise
        EVAL = FOUNDATION_MODULE_NAME + '.main(sys.argv[2:])'
	    
        if PROFILE:
            cProfile.run(EVAL, sort=1, filename="foundation.cprof")
            stats = pstats.Stats("foundation.cprof")
            stats.strip_dirs().sort_stats('time').print_stats(20)
        else:
            try:
                eval(EVAL)
            except KeyboardInterrupt, SystemExit:
                print "\n"
    
    else:
        print "[FoundationEngine] Error: Program", sys.argv[1], "doesn't exist."
		
else:
    print "[FoundationEngine] Error: You must run this as a standalone script."
    
print "[FoundationEngine] Shutdown complete."
