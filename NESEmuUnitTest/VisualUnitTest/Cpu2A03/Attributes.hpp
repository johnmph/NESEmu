//
//  Attributes.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/04/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef VisualUnitTest_Cpu2A03_Attributes_hpp
#define VisualUnitTest_Cpu2A03_Attributes_hpp

#include "../Cpu6502/Attributes.hpp"


namespace VisualUnitTest { namespace Cpu2A03 {
    
    template <class TCpu2A03>
    struct AttributeCAb : Attribute<TCpu2A03> {
        AttributeCAb(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu2A03 &cpu2A03) override;
        
    private:
        uint16_t _cAb;
    };
    
    template <class TCpu2A03>
    struct AttributeCDb : Attribute<TCpu2A03> {
        AttributeCDb(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu2A03 &cpu2A03) override;
        
    private:
        uint8_t _cDb;
    };
    
    template <class TCpu2A03>
    struct AttributeCRw : Attribute<TCpu2A03> {
        AttributeCRw(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu2A03 &cpu2A03) override;
        
    private:
        bool _cRw;
    };
    
    template <class TCpu2A03>
    std::unique_ptr<Attribute<TCpu2A03>> attributeFactory(std::string const &name);
    
    #include "Attributes_s.hpp"
    
} }

#endif /* VisualUnitTest_Cpu2A03_Attributes_hpp */
