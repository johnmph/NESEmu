//
//  Attribute_s.hpp
//  NESEmu
//
//  Created by Jonathan Baliko on 7/04/20.
//  Copyright © 2020 Jonathan Baliko. All rights reserved.
//

#ifndef Attribute_s_hpp
#define Attribute_s_hpp


template <class TCpu>
Attribute<TCpu>::Attribute(std::string const &name) : _name(name), _enableCheck(true) {
}

template <class TCpu>
std::string const &Attribute<TCpu>::getName() {
    return _name;
}

template <class TCpu>
void Attribute<TCpu>::setEnableCheck(bool enableCheck) {
    _enableCheck = enableCheck;
}

template <class TCpu>
bool Attribute<TCpu>::compareValue(TCpu &cpu) {
    if (_enableCheck) {
        return check(cpu);
    }
    
    return true;
}

template <class TCpu>
AttributeDontCheck<TCpu>::AttributeDontCheck(std::string const &name) : Attribute<TCpu>(name) {
}

template <class TCpu>
void AttributeDontCheck<TCpu>::setValue(std::string const &value) {
}

template <class TCpu>
bool AttributeDontCheck<TCpu>::check(TCpu &cpu) {
    return true;
}

#endif /* Attribute_s_hpp */
