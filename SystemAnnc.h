
// SystemAnnc Development Version

/*
     * Copyright 2012 Jack Deeth
     * Contact: simulationelectronics@gmail.com
     *
     * Simulation of Boeing 737 system annunciators, using SimLED
     * classes.
     *
     * This is unversioned development code, subject to rapid change
     * without notice and not always fully tested.
     *
     * This program is free software: you can redistribute it and/or modify
     * it under the terms of the GNU Lesser General Public License as
     * published by the Free Software Foundation, either version 3 of the
     * License, or (at your option) any later version.
     *
     * This program is distributed in the hope that it will be useful,
     * but WITHOUT ANY WARRANTY; without even the implied warranty of
     * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
     * GNU Lesser General Public License for more details.
     *
     * You should have received a copy of the GNU Lesser General Public
     * License along with this program.
     * If not, see <http://www.gnu.org/licenses/>.
     *
     * I would appreciate, but not insist, on attribution if this code is
     * incorporated into other projects.
     */

#ifndef SYSTEMANNC_H
#define SYSTEMANNC_H

// These includes likely won't work. Copy them into your main sketch
// source code.
#include "SimObjectsDev.h"
#include "SimLEDDev.h"

namespace b737 {
const int MAX_ANNCS_PER_SA = 12;
const int MAX_SA_PER_MC = 12;

//! System Annunciator class for Boeing 737
/*! Works in conjunction with the MasterCaution class.
 */
class SystemAnnc : public SimLEDBase {
public:

  //! Constructor
  /*! \param ledPin Arduino pin number of this SysAnnc's LED. Set to -1
   *         if not automatically outputting to an LED. (LED output can
   *         still be done using the .isLit() member function.)
   *  \param subAnncList list of SimLEDs (dataref-fed or local-logic-fed)
   *         which belong to this system and feed this System Annunciator
   *         They do not need to be linked to real LEDS (they can have a
   *         pin number of -1)
   *  \param sizeof_subAnncList This MUST be specified using sizeof(
   *         subAnncList) when the constructor is called!
   *  \param enableTest Should this SystemAnnc participate in generic
   *         SimLED bulb tests. Default is 'no' as the bulb test is
   *         provided by MasterCaution.recallMode(true).
   *  \param hasPowerFlag pointer to bool acting as simulated power
   *         supply for this SystemAnnc
   */
  SystemAnnc (const int    &ledPin,
         SimLEDBase   *subAnncList[],
         const size_t sizeof_subAnncList,
         const bool   &enableTest   = false,
         const bool   *hasPowerFlag = &SimObject::hasPower )
    : SimLEDBase(ledPin, enableTest, hasPowerFlag)
  {
    _subAnncs = subAnncList;
    _subAnncCount = sizeof_subAnncList / (sizeof(SimLEDBase*));
    if(_subAnncCount > MAX_ANNCS_PER_SA)
      _subAnncCount = MAX_ANNCS_PER_SA;
  }

  // needed so MasterCaution can call our _reset function
  friend class MasterCaution;

private:
  //! Pointer to array of SimLED sub-annunciators
  SimLEDBase **_subAnncs;

  //! Number of sub-annunciators feeding this SysAnnc
  unsigned short _subAnncCount;

  //! Record of which active subAnncs have been acknowledged as active
  bool _subAck[MAX_ANNCS_PER_SA];

  //! Recall mode lights the output regardless of subannc state
  bool _recallMode;

  //! True if any subanncs are active, regardless of ack'd status
  bool _hasActive;

  void _updateActive() {
    if (_recallMode) {
      _active = true;
      return;
    }
    _hasActive = false;
    for (int i = 0; i < _subAnncCount; ++i) {
      if(_subAnncs[i]->isActive()) {
        _hasActive = true;
        if(!_subAck[i]) {
          _active = true;
          _subAck[i] = true;
        }
      } else {
        _subAck[i] = false;
      }
    }
  } //_updateActive

  //! Deactivates subAnnc. Called by MasterCaution.
  void _reset() { _active = false; }

  //! Set recall mode on/off
  void _setRecall(bool mode) {
    // if we are starting recall mode
    if (mode && !_recallMode) {
      _recallMode = true;
    }

    // if we are ending Recall mode
    if (!mode && _recallMode) {
      // clear all acknowledgements
      for (int i = 0; i < _subAnncCount; ++i) {
        _subAck[i] = false;
      }
      _recallMode = false;
      _active = false;
    }
  }

};








//! Master Caution class for Boeing 737
/*! Works in conjunction with SystemAnnc
 */
class MasterCaution : public SimLEDBase {
public:
  //! Constructor
  /*! \param ledPin Arduino pin number for LED. Set to -1
   *         if not automatically outputting to an LED. (LED output can
   *         still be done using the .isLit() member function.)
   *  \param sysAnncList list of SystemAnncs which feed this Master
   *         Caution light. They do not need to be linked to real LEDS
   *         (they can have a pin number of -1)
   *  \param sizeof_sysAnncList This MUST be specified using sizeof(
   *         sysAnncList) when the constructor is called!
   *  \param enableTest Sets participation in generic
   *         SimLED bulb tests. Defaults to 'true'.
   *  \param hasPowerFlag pointer to bool acting as simulated power
   *         supply
   */
  MasterCaution (const int    &ledPin,
         SystemAnnc *sysAnncList[],
         const size_t sizeof_sysAnncList,
         const bool   &enableTest   = true,
         const bool   *hasPowerFlag = &SimObject::hasPower )
    : SimLEDBase(ledPin, enableTest, hasPowerFlag)
  {
    _sysAnncs = sysAnncList;
    _sysAnncCount = sizeof_sysAnncList / (sizeof(SystemAnnc*));
    if(_sysAnncCount > MAX_SA_PER_MC)
      _sysAnncCount = MAX_SA_PER_MC;
  }

  //! Reset all System Annunciators linked with this object
  void reset() {
    for (int i = 0; i < _sysAnncCount; ++i) {
      _sysAnncs[i]->_reset();
    }
    _active = false;
  }

  //! Set Recall mode for all System Annunciators linked with this object
  void setRecall(bool mode) {
    for (int i = 0; i < _sysAnncCount; ++i) {
      _sysAnncs[i]->_setRecall(mode);
    }
  }


private:
  //! Pointer to array of SystemAnnc associated with this MasterCaution
  SystemAnnc **_sysAnncs;

  //! Number of SystemAnnc linked to this MasterCaution
  unsigned short _sysAnncCount;

  //! MasterCaution is active if any of the fault lights are on
  void _updateActive() {
    for (int i = 0; i < _sysAnncCount; ++i) {
      if(_sysAnncs[i]->_hasActive) {
        _active = true;
      }
    }
  } //_updateActive


};

} //namespace b737SysAnnc

#endif // SYSTEMANNC_H
