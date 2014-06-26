#############################################################
#Compiling stuff
#############################################################
CXXFLAGS := -O3 -Wall -fPIC -g -fno-strict-aliasing

LDFLAGS := -Wall -g -ggdb -O3

#############################################################
#CACTUS Stuff
#############################################################

LIBRARIES =     -lreadline -lcurses

############################################################
#dependancy handling
#############################################################
DEP_DIR=./.dep



#############################################################
#AMC13Tool
#############################################################
AMC13TOOL=./bin/MezzTool
AMC13TOOL_DIR=src

AMC13TOOL_FILES=AMC13_CLI AMC13_Launcher AMC13_Launcher_commands mezz_tester mezz_tester_board \
serial_io MezzTool FDBlocking

AMC13TOOL_C=$(foreach file,$(AMC13TOOL_FILES),./$(AMC13TOOL_DIR)/$(file).cc)
AMC13TOOL_O=$(foreach file,$(AMC13TOOL_FILES),./build/$(file).o)
AMC13TOOL_D=$(foreach file,$(AMC13TOOL_FILES),./$(DEP_DIR)/$(file).d)

CXXFLAGS += -I$(AMC13TOOL_DIR)




#############################################################
#Builds
#############################################################

all:  $(AMC13TOOL)

clean :
	@rm -rf build/* >/dev/null
	@rm -rf $(AMC13TOOL)

distclean : clean
	@rm -rf $(DEP_DIR) > /dev/null

#########################################
#Dependancy builds
#########################################

$(DEP_DIR)/%.d : ./$(AMC13TOOL_DIR)/%.cc
	@mkdir -p $(DEP_DIR) > /dev/null
	@echo "Building dependency list for $^"
	@$(CXX) $(CXXFLAGS) -M -I./$(AMC13TOOL_DIR) $^ -MT build/$*.o -MF $(DEP_DIR)/$(@F).tmp > /dev/null
	@awk '{print $0}END{print "	$$(CXX) $$(CXXFLAGS) -c -o $$@ ./$(AMC13TOOL_DIR)/$*.cc"}' $(DEP_DIR)/$(@F).tmp > $(DEP_DIR)/$(@F)
	@rm $(DEP_DIR)/$(@F).tmp > /dev/null

-include  $(AMC13TOOL_D)

#########################################
#AMC13 Tool
#########################################
$(AMC13TOOL) : $(AMC13TOOL_D) $(AMC13TOOL_O)
	$(CXX) $(LDFLAGS) $(LIBRARY_PATH) -o $@ $(AMC13TOOL_O) $(LIBRARIES)


