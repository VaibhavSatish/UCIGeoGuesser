# __generated__ by Terraform
# Please review these resources and move them into your main configuration files.
# Modified by anivcs to protect sensitive information. See variables.tf and terraform.tfvars (gitignored) for the original values.

# __generated__ by Terraform from oci_core_instance.instance
resource "oci_core_instance" "instance" {
  async                      = null
  availability_domain        = "vkIa:US-SANJOSE-1-AD-1"
  cluster_placement_group_id = null
  compartment_id             = var.compartment_id
  defined_tags = {
    "Oracle-Tags.CreatedBy" = "default/${var.email}"
    "Oracle-Tags.CreatedOn" = "2026-06-03T19:58:28.439Z"
  }
  display_name      = "instance-20260603-1256"
  extended_metadata = {}
  fault_domain      = "FAULT-DOMAIN-1"
  freeform_tags     = {}
  metadata = {
    ssh_authorized_keys = var.ssh_public_key
  }
  preserve_boot_volume                    = null
  preserve_data_volumes_created_at_launch = null
  security_attributes                     = {}
  shape                                   = "VM.Standard.A1.Flex"
  state                                   = "RUNNING"
  update_operation_constraint             = null
  agent_config {
    are_all_plugins_disabled = false
    is_management_disabled   = false
    is_monitoring_disabled   = false
    plugins_config {
      desired_state = "DISABLED"
      name          = "Vulnerability Scanning"
    }
    plugins_config {
      desired_state = "DISABLED"
      name          = "OS Management Hub Agent"
    }
    plugins_config {
      desired_state = "DISABLED"
      name          = "Management Agent"
    }
    plugins_config {
      desired_state = "ENABLED"
      name          = "Custom Logs Monitoring"
    }
    plugins_config {
      desired_state = "DISABLED"
      name          = "Compute RDMA GPU Monitoring"
    }
    plugins_config {
      desired_state = "ENABLED"
      name          = "Compute Instance Monitoring"
    }
    plugins_config {
      desired_state = "DISABLED"
      name          = "Compute HPC RDMA Auto-Configuration"
    }
    plugins_config {
      desired_state = "DISABLED"
      name          = "Compute HPC RDMA Authentication"
    }
    plugins_config {
      desired_state = "ENABLED"
      name          = "Cloud Guard Workload Protection"
    }
    plugins_config {
      desired_state = "DISABLED"
      name          = "Block Volume Management"
    }
    plugins_config {
      desired_state = "DISABLED"
      name          = "Bastion"
    }
  }
  availability_config {
    is_live_migration_preferred = false
    recovery_action             = "RESTORE_INSTANCE"
  }
  create_vnic_details {
    assign_ipv6ip             = false
    assign_private_dns_record = false
    assign_public_ip          = "true"
    defined_tags = {
      "Oracle-Tags.CreatedBy" = "default/${var.email}"
      "Oracle-Tags.CreatedOn" = "2026-06-03T19:58:28.556Z"
    }
    display_name           = "mcserver"
    freeform_tags          = {}
    hostname_label         = "mcserver"
    nsg_ids                = []
    private_ip             = "10.0.0.152"
    security_attributes    = {}
    skip_source_dest_check = false
    subnet_id              = var.subnet_id
  }
  instance_options {
    are_legacy_imds_endpoints_disabled = true
  }
  launch_options {
    boot_volume_type                    = "PARAVIRTUALIZED"
    firmware                            = "UEFI_64"
    is_consistent_volume_naming_enabled = true
    is_pv_encryption_in_transit_enabled = true
    network_type                        = "PARAVIRTUALIZED"
    remote_data_volume_type             = "PARAVIRTUALIZED"
  }
  shape_config {
    baseline_ocpu_utilization = "BASELINE_1_1"
    local_volume_size_in_gbs  = 0
    memory_in_gbs             = 12
    nvmes                     = 0
    ocpus                     = 2
    vcpus                     = 2
  }
  source_details {
    boot_volume_size_in_gbs         = "47"
    boot_volume_vpus_per_gb         = "10"
    is_preserve_boot_volume_enabled = false
    kms_key_id                      = null
    source_id                       = var.source_id
    source_type                     = "image"
  }
}
