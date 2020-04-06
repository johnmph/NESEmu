//
//  Visual6502.hpp
//  NESEmuUnitTest
//
//  Created by Jonathan Baliko on 6/02/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Visual6502_hpp
#define Visual6502_hpp

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
#include <istream>


namespace Visual6502 {
    
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
    
    template <class TCpu6502>
    struct Attribute {
        Attribute(std::string const &name);
        virtual ~Attribute();
        
        std::string const &getName();
        
        void setEnableCheck(bool enableCheck);
        
        bool compareValue(TCpu6502 &cpu6502);
        
        virtual void setValue(std::string const &value) = 0;
        
    protected:
        virtual bool check(TCpu6502 &cpu6502) = 0;
        
    private:
        std::string _name;
        bool _enableCheck;
    };
    
    template <class TCpu6502>
    struct AttributeDontCheck : Attribute<TCpu6502> {
        AttributeDontCheck(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
    };
    
    template <class TCpu6502>
    struct AttributeAb : Attribute<TCpu6502> {
        AttributeAb(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint16_t _ab;
    };
    
    template <class TCpu6502>
    struct AttributeDb : Attribute<TCpu6502> {
        AttributeDb(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _db;
    };
    
    template <class TCpu6502>
    struct AttributeRw : Attribute<TCpu6502> {
        AttributeRw(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        bool _rw;
    };
    
    template <class TCpu6502>
    struct AttributePc : Attribute<TCpu6502> {
        AttributePc(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint16_t _pc;
    };
    
    template <class TCpu6502>
    struct AttributePcl : Attribute<TCpu6502> {
        AttributePcl(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _pcl;
    };
    
    template <class TCpu6502>
    struct AttributePch : Attribute<TCpu6502> {
        AttributePch(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _pch;
    };
    
    template <class TCpu6502>
    struct AttributeA : Attribute<TCpu6502> {
        AttributeA(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _a;
    };
    
    template <class TCpu6502>
    struct AttributeX : Attribute<TCpu6502> {
        AttributeX(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _x;
    };
    
    template <class TCpu6502>
    struct AttributeY : Attribute<TCpu6502> {
        AttributeY(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _y;
    };
    
    template <class TCpu6502>
    struct AttributeS : Attribute<TCpu6502> {
        AttributeS(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _s;
    };
    
    template <class TCpu6502>
    struct AttributeP : Attribute<TCpu6502> {
        AttributeP(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        uint8_t _p;
    };
    
    template <class TCpu6502>
    struct AttributeSync : Attribute<TCpu6502> {
        AttributeSync(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        bool _sync;
    };
    
    template <class TCpu6502>
    struct AttributeReadyLine : Attribute<TCpu6502> {
        AttributeReadyLine(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu6502 &cpu6502) override;
        
    private:
        bool _readyLine;
    };
    
    template <class TCpu6502>
    struct Attributes {
        std::vector<std::unique_ptr<Attribute<TCpu6502>>> attributes;
    };
    
    
    template <class TCpu6502>
    struct Analyzer {
        template <class TFunction>
        Analyzer(std::istream &istream, TFunction &&writeFunction);
        
        template <class TFunction>
        void analyze(TCpu6502 &cpu6502, TFunction &&checkResult, int startCycle = 0);
        
    private:
        std::unique_ptr<Attribute<TCpu6502>> makeAttributeFromName(std::string const &name);
        
        void decodeUrlParameter(std::string const &name, std::string const &value, UrlCommand &command);
        void decodeUrl(std::string const &url);
        void decodeAttributes(std::string attributesString);
        std::vector<std::string> decodeResults(std::string resultsString);
        
        std::istream &_istream;
        UrlCommand _command;
        Attributes<TCpu6502> _attributes;
    };
    
    
    #include "Visual6502_s.hpp"
    
}

#endif /* Visual6502_hpp */
