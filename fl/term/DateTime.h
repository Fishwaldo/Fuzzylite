/*   Copyright 2014 Justin Hammond

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

/* 
 * File:   DateTime.h
 * Author: justin hammond
 *
 * Created on 13/8/2014
 */

#ifndef FL_TIME_H
#define	FL_TIME_H

#include "fl/term/Term.h"

namespace fl {

    class FL_EXPORT DateTime : public Term {
    public:
    typedef enum TIME_TYPE {
    	TIME_GREATERTHAN,
    	TIME_LESSTHAN,
    	TIME_EQUALTO
    } TIME_TYPE;


    protected:
        scalar _value;

    public:
        DateTime(const std::string& name = "", TIME_TYPE type = TIME_GREATERTHAN);
        virtual ~DateTime();
        
        virtual std::string className() const;
        virtual std::string parameters() const;
        virtual void configure(const std::string& parameters);
        
        virtual scalar membership(scalar x) const;

        virtual void setValue(scalar value);
        virtual scalar getValue() const;
        
        virtual DateTime* copy() const;
        
        static Term* constructor();
        bool hasArgs();
        void setArgs(std::string param);
        std::string getArgs();
    private:
        std::string arg;
        TIME_TYPE type;
    };
}

#endif	/* FL_TIME_H */

