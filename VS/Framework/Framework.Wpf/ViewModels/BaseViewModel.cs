////////////////////////////////////////////////////////
/*
  This file is part of CNCLib - A library for stepper motors.

  Copyright (c) 2013-2017 Herbert Aitenbichler

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

using System;
using System.Windows;
using System.Runtime.CompilerServices;
using System.Threading.Tasks;

namespace Framework.Wpf.ViewModels
{
    public class BaseViewModel : Prism.Mvvm.BindableBase // NotificationObject
	{
		public Action CloseAction { get; set; }

		public Func<string, string, MessageBoxButton, MessageBoxImage, MessageBoxResult>MessageBox{ get; set; }

		public virtual void Cleanup()
		{

		}
		public virtual async Task Loaded()
		{
			await Task.FromResult(0);
		}

		protected bool SetProperty(Func<bool> equal, Action action, [CallerMemberName] string propertyName = null)
		{
			if (equal())
			{
				return false;
			}

			RaisePropertyChanged(action, propertyName);

			return true;
		}

		protected void RaisePropertyChanged(Action action, [CallerMemberName] string propertyName = null)
		{
			action();
            RaisePropertyChanged(propertyName);
		}
	}
}