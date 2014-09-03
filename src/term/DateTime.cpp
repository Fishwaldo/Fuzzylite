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
 * File:   DateTime.cpp
 * Author: Justin Hammond
 *
 * Created on 13/8/2014
 */

#include "fl/term/DateTime.h"
#include "fl/util/approxidate.h"

namespace fl {

    DateTime::DateTime(const std::string& name, TIME_TYPE type)
    : Term(name), _value(0), type(type) {
    }

    DateTime::~DateTime() {
    }

    std::string DateTime::className() const {
        return "DateTime";
    }

    std::string DateTime::parameters() const {
        switch (this->type) {
        	case TIME_GREATERTHAN:
        		return "TIME_GREATERTHAN";
        	case TIME_LESSTHAN:
        		return "TIME_LESSTHAN";
        	case TIME_EQUALTO:
        		return "TIME_EQUALTO";
        }
        return "Unknown";
    }

    void DateTime::configure(const std::string& parameters) {
        if (parameters.empty()) return;
        if (fl::icasecmp("TIME_GREATERTHAN", parameters)) {
        	this->type = TIME_GREATERTHAN;
        } else if (fl::icasecmp("TIME_LESSTHAN", parameters)) {
        	this->type = TIME_LESSTHAN;
        } else if (fl::icasecmp("TIME_EQUALTO", parameters)) {
        	this->type = TIME_EQUALTO;
        } else {
            std::ostringstream ex;
            ex << "[configuration error] term <" << className() << ">"
                    << " requires a Valid parameter";
            throw fl::Exception(ex.str(), FL_AT);
        }
        //setValue(Op::toScalar(parameters));
    }

    scalar DateTime::membership(scalar x) const {
        //FL_DBG("Membership: " << Op::str(x) << " Against: " << Op::str(this->_value));
        switch (this->type) {
        	case TIME_GREATERTHAN:
        		return (x > this->_value);
        	case TIME_LESSTHAN:
        		return (x < this->_value);
        	case TIME_EQUALTO:
        		/* XXX TODO - Remove Seconds */
        		return (x < this->_value);
        }
        return 0;
    }

    void DateTime::setValue(scalar value) {
        this->_value = value;
    }

    scalar DateTime::getValue() const {
        return this->_value;
    }

    DateTime* DateTime::copy() const {
        return new DateTime(*this);
    }

    Term* DateTime::constructor() {
        return new DateTime;
    }
    bool DateTime::hasArgs() {
    	return true;
    }
    void DateTime::setArgs(std::string param) {
    	this->arg = param;
    	struct timeval tv;
    	if (approxidate(this->arg.c_str(), &tv) == 0) {
            struct tm *nowtm;
            char tmbuf[64];
            nowtm = localtime(&tv.tv_sec);
            strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", nowtm);
            FL_DBG("Parsed Time/Date: " << tmbuf << " from " << param);
            this->setValue(tv.tv_sec);
    	} else {
            std::ostringstream ex;
            ex << "[syntax error] Can't Parse Time Value: "
                    << this->arg;
            throw fl::Exception(ex.str(), FL_AT);
    	}
    }
    std::string DateTime::getArgs() {
        return this->arg;
    }

}
