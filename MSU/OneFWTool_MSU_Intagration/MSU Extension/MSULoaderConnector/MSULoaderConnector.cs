using Microsoft.Practices.Prism.Logging;
using Microsoft.Practices.Prism.MefExtensions.Modularity;
using Microsoft.Practices.Prism.Modularity;
using Microsoft.Practices.Prism.Regions;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;

namespace se.onefwtool.MSULoaderConnector
{
    /// <summary>
    /// A module for the connector
    /// </summary>
    [ModuleExport(typeof(MSULoaderConnector))]
    public class MSULoaderConnector : IModule
    {
        #region Variables
        private readonly ILoggerFacade _logger;
        private readonly IRegionManager _regionManager;
        #endregion

        #region Methods
        /// <summary>
        /// Initializes a new instance.
        /// </summary>
        [ImportingConstructor]
        public MSULoaderConnector(ILoggerFacade logger, IRegionManager regionManager)
        {
            _logger = logger;
            _regionManager = regionManager;
        }

        /// <summary>
        /// Notifies the module that it has be initialized.
        /// </summary>
        public void Initialize()
        {
            _logger.Log("MSULoaderConnector - Initialize", Category.Info, Priority.Medium);
        }
        #endregion
    }
}

