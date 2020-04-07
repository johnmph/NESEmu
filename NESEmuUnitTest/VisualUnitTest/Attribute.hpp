//
//  Attribute.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/04/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef VisualUnitTest_Attribute_hpp
#define VisualUnitTest_Attribute_hpp

#include <string>
#include <vector>
#include <memory>


namespace VisualUnitTest {
    
    template <class TCpu>
    struct Attribute {
        Attribute(std::string const &name);
        virtual ~Attribute();
        
        std::string const &getName();
        
        void setEnableCheck(bool enableCheck);
        
        bool compareValue(TCpu &cpu);
        
        virtual void setValue(std::string const &value) = 0;
        
    protected:
        virtual bool check(TCpu &cpu) = 0;
        
    private:
        std::string _name;
        bool _enableCheck;
    };
    
    template <class TCpu>
    struct AttributeDontCheck : Attribute<TCpu> {
        AttributeDontCheck(std::string const &name);
        
        void setValue(std::string const &value) override;
        bool check(TCpu &cpu) override;
    };
    
    template <class TCpu>
    struct Attributes {
        std::vector<std::unique_ptr<Attribute<TCpu>>> attributes;
    };
    
    #include "Attribute_s.hpp"
    
}

#endif /* Attribute_hpp */
