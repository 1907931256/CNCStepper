﻿////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2016 Herbert Aitenbichler

  CNCLib is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  CNCLib is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  http://www.gnu.org/licenses/
*/

using Framework.Tools;
using CNCLib.Logic.Contracts.DTO;
using Framework.Tools.Dependency;
using Framework.Tools.Mapper;
using AutoMapper;

namespace CNCLib.Logic.Converter
{
	static class Converter
	{
		public static Machine Convert(this Repository.Contracts.Entities.Machine from)
		{
			var map = Dependency.Resolve<IMapper>();
			return map.Map<Machine>(from);

			//var dto =	ObjectConverter.NewCloneProperties<Machine, Repository.Contracts.Entities.Machine>(from);
			//dto.MachineCommands = ObjectConverter.CloneAsList<MachineCommand, Repository.Contracts.Entities.MachineCommand>(from.MachineCommands);
			//dto.MachineInitCommands = ObjectConverter.CloneAsList<MachineInitCommand, Repository.Contracts.Entities.MachineInitCommand>(from.MachineInitCommands);
			//return dto;
		}

		public static Repository.Contracts.Entities.Machine Convert(this Machine from)
		{
			var map = Dependency.Resolve<IMapper>();
			return map.Map<Repository.Contracts.Entities.Machine>(from);

			//var entity = ObjectConverter.NewCloneProperties<Repository.Contracts.Entities.Machine, Machine>(from);
			//entity.MachineCommands = ObjectConverter.CloneAsList<Repository.Contracts.Entities.MachineCommand, MachineCommand>(from.MachineCommands);
			//entity.MachineInitCommands = ObjectConverter.CloneAsList<Repository.Contracts.Entities.MachineInitCommand, MachineInitCommand>(from.MachineInitCommands);
			//return entity;
		}

        public static Item Convert(this Repository.Contracts.Entities.Item from)
        {
			var map = Dependency.Resolve<IMapper>();
			return map.Map<Item>(from);

//			var dto = ObjectConverter.NewCloneProperties<Item, Repository.Contracts.Entities.Item>(from);
//            return dto;
        }

    }
}
