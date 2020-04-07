//
//  Analyzer.hpp
//  NESEmuUnitTest
//
//  Created by Jonathan Baliko on 6/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef VisualUnitTest_Analyzer_hpp
#define VisualUnitTest_Analyzer_hpp

#include <vector>
#include <unordered_map>
#include <utility>
#include <istream>
#include "Attribute.hpp"


namespace VisualUnitTest {
    
    struct UrlCommand {
        UrlCommand();
        
        std::unordered_map<std::string, std::vector<std::pair<int, int>>> checkAttribute;
        std::vector<std::pair<int, std::vector<uint8_t>>> data;
        int numCyclesToExecute;
        std::vector<std::pair<int, int>> resetLine;
        std::vector<std::pair<int, int>> nmiLine;
        std::vector<std::pair<int, int>> irqLine;
        std::vector<std::pair<int, int>> soLine;
        std::vector<std::pair<int, int>> rdyLine;
    };
    
    template <class TCpu>
    struct Analyzer {
        template <class TAttributeFactoryFunction, class TWriteFunction>
        Analyzer(std::istream &istream, TAttributeFactoryFunction &&attributeFactoryFunction, TWriteFunction &&writeFunction);
        
        template <class TFunction>
        void analyze(TCpu &cpu, TFunction &&checkResult, int startCycle = 0);
        
    private:
        void decodeUrlParameter(std::string const &name, std::string const &value, UrlCommand &command);
        void decodeUrl(std::string const &url);
        
        template <class TAttributeFactoryFunction>
        void decodeAttributes(TAttributeFactoryFunction &&attributeFactoryFunction, std::string attributesString);
        
        std::vector<std::string> decodeResults(std::string resultsString);
        
        std::istream &_istream;
        UrlCommand _command;
        Attributes<TCpu> _attributes;
    };
    
    #include "Analyzer_s.hpp"
    
}

#endif /* VisualUnitTest_Analyzer_hpp */
