using Microsoft.Practices.Prism;
using Microsoft.Practices.Prism.Events;
using Microsoft.Practices.Prism.Logging;
using Microsoft.Practices.Prism.Regions;
using se.onefwtool.common;
using se.onefwtool.common.DataTypes;
using se.onefwtool.common.Serialization;
using se.onefwtool.Infrastructure;
using se.onefwtool.Infrastructure.Services;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;

namespace se.onefwtool.MSULoaderConnector.Models
{
    /// <summary>
    /// Business Model for the MSU connector
    /// </summary>
    [Export(typeof(IFirmwareModule))]
    [PartCreationPolicy(CreationPolicy.Shared)]
    public class MSUBusinessModel : IFirmwareModule
    {
                #region Variables
        private readonly ILoggerFacade _Logger;
        private readonly IRegionManager _RegionManager;
        private readonly IEventAggregator _EventAggrgeator;
        #endregion

        #region Methods
        /// <summary>
        /// Initializes a new instance.
        /// </summary>
        [ImportingConstructor]
        public MSUBusinessModel(ILoggerFacade logger, IEventAggregator events, IRegionManager region)
        {
            _Logger = logger;
            _EventAggrgeator = events;
            _RegionManager = region;

            _Logger.Log("MSUBusinessModel - Initialize", Category.Info, Priority.Medium);
        }
        #endregion

        #region Interface
        /// <summary>
        /// Returns information about the module
        /// </summary>
        public int GetModuleInformation(out ModuleInfoType info)
        {
            info = new ModuleInfoType();
            info.Name = "MSULoader";
            return (int)StatusCodes.OK;
        }

        /// <summary>
        /// Opens the dedicated view of the loader
        /// </summary>
        public int OpenLoader(PackageType package, List<string> addresses, int protocol, string id)
        {
            string packageString = string.Empty;
            int status = SerializationString.ExportToXMLString<PackageType>(package, out packageString);
            if (status  == (int)StatusCodes.OK)
            {
                var uriQuery = new UriQuery();
                uriQuery.Add("id", id);
                uriQuery.Add("protocol", protocol.ToString());
                uriQuery.Add("package", packageString);
                uriQuery.Add("addresses", string.Join(",", addresses.ToArray()));

                var uri = new Uri("MSUView" + uriQuery.ToString(), UriKind.Relative);
                IRegion region = _RegionManager.Regions[RegionNames.ContentRegion];
                region.RequestNavigate(uri);
            }
            return status;
        }
        #endregion
    }
}
