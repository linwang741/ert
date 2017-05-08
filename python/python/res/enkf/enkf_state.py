#  Copyright (C) 2012  Statoil ASA, Norway. 
#   
#  The file 'enkf_state.py' is part of ERT - Ensemble based Reservoir Tool. 
#   
#  ERT is free software: you can redistribute it and/or modify 
#  it under the terms of the GNU General Public License as published by 
#  the Free Software Foundation, either version 3 of the License, or 
#  (at your option) any later version. 
#   
#  ERT is distributed in the hope that it will be useful, but WITHOUT ANY 
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or 
#  FITNESS FOR A PARTICULAR PURPOSE.   
#   
#  See the GNU General Public License at <http://www.gnu.org/licenses/gpl.html> 
#  for more details.
from cwrap import BaseCClass
from res.enkf import EnkfPrototype
from res.enkf.enums import EnkfInitModeEnum, EnkfVarType
from res.job_queue import JobStatusType


class EnKFState(BaseCClass):
    TYPE_NAME       = "enkf_state"
    _free           = EnkfPrototype("void* enkf_state_free( enkf_state )")
    _add_subst_kw   = EnkfPrototype("void enkf_state_add_subst_kw( enkf_state , char* , char* , char*)")
    _get_subst_list = EnkfPrototype("subst_list_ref enkf_state_get_subst_list( enkf_state )")
    _get_ens_config = EnkfPrototype("ens_config_ref enkf_state_get_ensemble_config( enkf_state )")
    _initialize     = EnkfPrototype("void enkf_state_initialize( enkf_state , enkf_fs , stringlist , enkf_init_mode_enum)")
    
    def __init__(self):
        raise NotImplementedError("Class can not be instantiated directly!")
        
    

    def free(self):
        self._free( )


    def addSubstKeyword(self , key , value):
        """
        Will add a key -> value pair which can be used for search replace
        operations in the data file. Observe that the key will be
        surrounded by \'<\' and \'>\'.
        """
        doc_string = None
        if isinstance(value , str):
            self._add_subst_kw( key , value , doc_string )
        else:
            raise TypeError("The value argument must be a string")

    def getDataKW(self):
        """
        Will return the substitution map for this realisation.
        """
        return self._get_subst_list( )


    def ensembleConfig(self):
        """ @rtype: EnsembleConfig """
        return self._get_ens_config( )
    def initialize( self , fs , param_list = None , init_mode = EnkfInitModeEnum.INIT_CONDITIONAL):
        if param_list is None:
            ens_config = self.ensembleConfig( )
            param_list = ens_config.getKeylistFromVarType( EnkfVarType.PARAMETER )
        self._initialize( fs , param_list , init_mode )
