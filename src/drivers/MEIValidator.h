/*
 * MEIValidator.h
 *
 *  Created on: Apr 9, 2019
 *      Author: mark
 */

#ifndef DRIVERS_MEIVALIDATOR_H_
#define DRIVERS_MEIVALIDATOR_H_

class MEI_Validator {



public:
	MEI_Validator(cmd);
	virtual ~MEI_Validator(cmd);
	char * mei_cmd();
};

#endif /* DRIVERS_MEIVALIDATOR_H_ */
