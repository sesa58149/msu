using Microsoft.Practices.Prism.Logging;
using se.onefwtool.MSULoaderConnector.ViewModels;
using System;
using System.Collections.Generic;
using System.ComponentModel.Composition;
using System.Linq;
using System.Windows.Controls;

namespace se.onefwtool.MSULoaderConnector.Views
{
    /// <summary>
    /// Interaction logic for MSUView.xaml
    /// </summary>
    [Export("MSUView")]
    public partial class MSUView : UserControl
    {
        [ImportingConstructor]
        public MSUView(ILoggerFacade logger)
        {
            InitializeComponent();
            logger.Log("MSUViewModule - View Initialize", Category.Info, Priority.Medium);
        }

        [Import(AllowRecomposition = false)]
        public MSUViewModel ViewModel
        {
            get { return this.DataContext as MSUViewModel; }
            set { this.DataContext = value; }
        }
    }
}
