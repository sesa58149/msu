using Ionic.Zip;
using Microsoft.Practices.Prism.Events;
using Microsoft.Practices.Prism.Logging;
using Microsoft.Practices.Prism.Regions;
using se.onefwtool.common;
using se.onefwtool.common.DataTypes;
using se.onefwtool.common.Serialization;
using se.onefwtool.Infrastructure;
using se.onefwtool.Infrastructure.Models;
using se.onefwtool.MSULoaderConnector.Tools;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.IO;
using System.Linq;
using System.Threading;
using System.Windows.Threading;

namespace se.onefwtool.MSULoaderConnector.ViewModels
{
    [Export]
    public class MSUViewModel : BaseViewModel, INavigationAware
    {
        #region Variables
       // Framework specific variables
        private readonly IRegionManager _regionManager;
        private static Dispatcher _dispatcher;

        // Parameter coming from the FW Tool
        private string _id; // Internal id
        private List<string> _deviceAddresses = new List<string>();
        private PackageType _packageInformation;
        private bool _LoadingDataInvalid;

        private string _ExtractedPackageFilesDirectory;

        // MSU specific variables
        private bool _isConnected;
        private bool _isBusy;
        #endregion

        #region Properties
        public bool IsConnected
        {
            get { return _isConnected; }
            set
            {
                _isConnected = value;
                OnPropertyChanged("IsConnected");
            }
        }
        public bool IsBusy
        {
            get { return _isBusy; }
            set
            {
                _isBusy = value;
                OnPropertyChanged("IsBusy");
            }
        }
        #endregion

        #region Commands
        /// <summary>
        /// Executes a command comming from the UI
        /// </summary>
        private RelayCommand _DoSomethingCommand;
        public RelayCommand DoSomethingCommand
        {
            get
            {
                if (_DoSomethingCommand != null) return _DoSomethingCommand;
                _DoSomethingCommand = new RelayCommand(a => OnDoSomething());
                return _DoSomethingCommand;
            }
        }
        private void OnDoSomething()
        {
            if (IsConnected == true)
            {
                IsConnected = false;
            }
            else
            {
                IsConnected = true;
            }
        }
        #endregion
    
        #region Methods
        /// <summary>
        /// Constructor
        /// </summary>
        [ImportingConstructor]
        public MSUViewModel(ILoggerFacade logger, IEventAggregator events, IRegionManager regions)
            : base(logger, events)
        {
            _dispatcher = Dispatcher.CurrentDispatcher;
            _regionManager = regions;
            Logger.Log("MSUModule - ViewModel Initialize", Category.Info, Priority.Medium);
        }

        /// <summary>
        /// Send a status event to the shell which is display in the right upper corner.
        /// </summary>
        private void SendStatus(StatusEventType type, string message)
        {
            EventAggregator.GetEvent<GlobalStatusEvent>().Publish(new StatusEvent()
            {
                Message = message,
                Type = type
            });
        }

        /// <summary>
        /// Extracts the firmware files into temp directory
        /// </summary>
        private void ExtractPackage(string packageName, string packagePath, out string unpackDirectory)
        {
            unpackDirectory = string.Empty;

            if (File.Exists(packagePath))
            {
                string zipToUnpack = packagePath;
                unpackDirectory = string.Format("{0}{1}", Constants.TempDirectory, packageName);
                using (ZipFile zip1 = ZipFile.Read(zipToUnpack))
                {
                    foreach (ZipEntry e in zip1)
                    {
                        e.Extract(unpackDirectory, ExtractExistingFileAction.OverwriteSilently);
                    }
                }
            }
            else
            {
                SendStatus(StatusEventType.Error, "Package file does ot exist");
                Logger.Log(string.Format("UnityLoaderModule - Package file does ot exist: {0}", packagePath), Category.Info, Priority.Medium);
            }
        }

        /// <summary>
        /// Deletes the unpacked data
        /// </summary>
        private void DeleteTempPackageData(string unpackDirectory)
        {
            if (Directory.Exists(unpackDirectory))
            {
                Directory.Delete(unpackDirectory, true);
            }
        }

        /// <summary>
        /// Extracts the package files.
        /// </summary>
        private void ExtractPackageFiles(string packageName, string packageFullpath)
        { 
            // Extract the package files
            ThreadStart start = delegate
            {
                IsBusy = true;
                ExtractPackage(packageName, packageFullpath, out _ExtractedPackageFilesDirectory);
                IsBusy = false;
            };
            Thread thread = new Thread(start);
            thread.Start();   
 
        }
        #endregion
   
        #region Navigation
        /// <summary>
        /// Checks if the navigation is enabled/disabled.
        /// </summary>
        /// <param name="navigationContext">Navigation context.</param>
        /// <returns>Returns true if is enabled otherwise false.</returns>
        public bool IsNavigationTarget(NavigationContext navigationContext)
        {
            return true;
        }

        /// <summary>
        /// Is called when navigation focus leaves current view.
        /// </summary>
        /// <param name="navigationContext">Navigation context.</param>
        public void OnNavigatedFrom(NavigationContext navigationContext)
        { }

        /// <summary>
        /// Is called when navigation focus enters current view.
        /// </summary>
        /// <param name="navigationContext">Navigation context.</param>
        public void OnNavigatedTo(NavigationContext navigationContext)
        {
            IsConnected = false;
            _LoadingDataInvalid = false;
           
            // Get data from context
            var id = navigationContext.Parameters["id"];
            if (!string.IsNullOrEmpty(id))
            {
                _id = id;
            }
            else
            {
                _LoadingDataInvalid = true;
                _id = string.Empty;
            }

            _deviceAddresses.Clear(); 
            var address = navigationContext.Parameters["addresses"];
            if (!string.IsNullOrEmpty(address))
            {
                _deviceAddresses = new List<string>(address.Split(','));
            }
            else
            {
                _LoadingDataInvalid = true;
            }

            var package = navigationContext.Parameters["package"];
            if (!string.IsNullOrEmpty(package))
            {
                int status = SerializationString.ImportFromXMLString<PackageType>(package, out _packageInformation);
                if (status == (int)StatusCodes.OK || _packageInformation != null)
                {
                    ExtractPackageFiles(_packageInformation.Name, _packageInformation.FullPath);
                }
                else
                { 
                     _LoadingDataInvalid = true;
                    _packageInformation = null;
                }
            }
            else
            {
                _LoadingDataInvalid = true;
                _packageInformation = null;
            }
        }
        #endregion
    }
}
